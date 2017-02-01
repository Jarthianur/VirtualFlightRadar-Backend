/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
 A detailed list of copyright holders can be found in the file "AUTHORS".

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License version 3
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 }
 */

#include "VFRB.h"

#include <chrono>
#include <csignal>
#include <exception>
#include <functional>
#include <string>
#include <system_error>
#include <thread>

#include "../aircraft/AircraftContainer.h"
#include "../aircraft/AircraftProcessor.h"
#include "../connection/ConnectionException.h"
#include "../connection/ConnectIn.h"
#include "../connection/ConnectInExt.h"
#include "../parser/ParserAPRS.h"
#include "../parser/ParserSBS.h"
#include "../util/Logger.h"
#include "Configuration.h"
#include "WeatherFeed.h"

bool VFRB::global_weather_feed_enabled = false;
bool VFRB::global_aprs_enabled = false;
bool VFRB::global_sbs_enabled = false;
bool VFRB::global_run_status = true;

VFRB::VFRB()
{
}

VFRB::~VFRB()
{
}

void VFRB::run()
{
    ConnectOutNMEA out_con(Configuration::global_out_port);
    AircraftContainer ac_cont;
    WeatherFeed weather_feed;
    AircraftProcessor ac_proc(Configuration::base_latitude, Configuration::base_longitude,
                              Configuration::base_altitude, Configuration::base_geoid);

    if (Configuration::global_aprsc_host.compare("nA") != 0 || Configuration::global_aprsc_host.length()
            == 0)
    {
        global_aprs_enabled = true;
    }
    else
    {
        Logger::warn("APRSC not enabled -> no FLARM targets");
    }

    if (Configuration::global_sbs_host.compare("nA") != 0 || Configuration::global_sbs_host.length()
            == 0)
    {
        global_sbs_enabled = true;
    }
    else
    {
        Logger::warn("ADSB receiver not enabled -> no transponder targets");
    }

    if (Configuration::global_weather_feed_host.compare("nA") != 0 || Configuration::global_weather_feed_host.length()
            == 0)
    {
        global_weather_feed_enabled = true;
    }
    else
    {
        Logger::warn("Weather feed not enabled -> no wind,pressure,temp");
    }

    struct sigaction sa;
    sa.sa_handler = VFRB::exit_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        Logger::error("Failed to register signal: SIGINT");
        global_run_status = false;
        return;
    }

    std::thread sbs_in_thread(handle_sbs_in, std::ref(ac_cont));
    std::thread aprs_in_thread(handle_aprs_in, std::ref(ac_cont));
    std::thread con_out_thread(handle_con_out, std::ref(out_con));
    std::thread weather_feed_thread(handle_weather_feed, std::ref(weather_feed));

    while (global_run_status)
    {
        try
        {
            std::string str = ac_cont.processAircrafts();
            if (str.length() > 0)
            {
                out_con.sendMsgOut(std::ref(str));
            }

            str = ac_proc.gpsfix();
            out_con.sendMsgOut(std::ref(str));

            if (global_weather_feed_enabled)
            {
                if (weather_feed.getPress() != A_VALUE_NA)
                {
                    Configuration::base_pressure = weather_feed.getPress();
                }
                if (weather_feed.getTemp() != A_VALUE_NA)
                {
                    Configuration::base_temp = weather_feed.getTemp();
                }
                if (weather_feed.isValid())
                {
                    str = weather_feed.getNMEA();
                    out_con.sendMsgOut(std::ref(str));
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(SYNC_TIME));
        }
        catch (const std::exception& e)
        {
            Logger::error("Fatal error -> terminating program:  ", e.what());
            global_run_status = false;
            break;
        }
    }

    try
    {
        sbs_in_thread.join();
        aprs_in_thread.join();
        con_out_thread.join();
        weather_feed_thread.join();
    }
    catch (const std::system_error& se)
    {
        Logger::error("while joining threads: ", se.what());
        std::terminate();
    }
    Logger::info("EXITING PROGRAM");
}

void VFRB::handle_con_out(ConnectOutNMEA& out_con)
{
    try
    {
        out_con.listenOut();
    }
    catch (const ConnectionException& ce)
    {
        Logger::error("while setup out-connection: ", ce.what());
        global_run_status = false;
        return;
    }
    Logger::info("Serve NMEA output on localhost:",
                 std::to_string(Configuration::global_out_port));
    while (global_run_status)
    {
        out_con.connectClient();
    }
}

void VFRB::handle_weather_feed(WeatherFeed& weather)
{
    if (!global_weather_feed_enabled)
    {
        return;
    }
    ConnectIn wind_con(std::ref(Configuration::global_weather_feed_host),
                       Configuration::global_weather_feed_port, 5);
    bool connected = false;

    while (global_weather_feed_enabled && global_run_status)
    {
        while (!connected && global_run_status)
        {
            wind_con.close();
            try
            {
                wind_con.setupConnectIn();
            }
            catch (const ConnectionException& ce)
            {
                Logger::error("Failed to setup weather input connection: ", ce.what());
                global_weather_feed_enabled = false;
                return;
            }
            try
            {
                wind_con.connectIn();
                connected = true;
            }
            catch (const ConnectionException& ce)
            {
                Logger::error("Failed to connect to weather input: ", ce.what());
                std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
            }
        }
        if (wind_con.readLineIn() == CI_MSG_READ_ERR)
        {
            connected = false;
        }
        else
        {
            weather.writeNMEA(std::ref(wind_con.getResponse()));
        }
    }
}

void VFRB::handle_sbs_in(AircraftContainer& ac_cont)
{
    if (!global_sbs_enabled)
    {
        return;
    }
    ParserSBS parser;
    ConnectIn sbs_con(std::ref(Configuration::global_sbs_host),
                      Configuration::global_sbs_port);
    bool connected = false;

    while (global_sbs_enabled && global_run_status)
    {
        while (!connected && global_run_status)
        {
            sbs_con.close();
            try
            {
                sbs_con.setupConnectIn();
            }
            catch (const ConnectionException& ce)
            {
                Logger::error("Failed to setup SBS input connection: ", ce.what());
                global_sbs_enabled = false;
                return;
            }
            try
            {
                sbs_con.connectIn();
                connected = true;
                Logger::info("Scan for incoming sbs-msgs from ",
                             Configuration::global_sbs_host);
            }
            catch (const ConnectionException& ce)
            {
                Logger::error("Failed to connect to SBS input: ", ce.what());
                std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
            }
        }
        if (sbs_con.readLineIn() == CI_MSG_READ_ERR)
        {
            connected = false;
        }
        //need msg3 only
        else if (sbs_con.getResponse().at(4) == '3')
        {
            parser.unpack(std::ref(sbs_con.getResponse()), std::ref(ac_cont));
        }
    }
}

void VFRB::handle_aprs_in(AircraftContainer& ac_cont)
{
    if (!global_aprs_enabled)
    {
        return;
    }
    ParserAPRS parser;
    ConnectInExt aprs_con(std::ref(Configuration::global_aprsc_host),
                          Configuration::global_aprsc_port,
                          std::ref(Configuration::global_aprsc_login));
    bool connected = false;

    while (global_aprs_enabled && global_run_status)
    {
        while (!connected && global_run_status)
        {
            aprs_con.close();
            try
            {
                aprs_con.setupConnectIn();
            }
            catch (const ConnectionException& ce)
            {
                Logger::error("Failed to setup APRS input connection: ", ce.what());
                global_aprs_enabled = false;
                return;
            }
            try
            {
                aprs_con.connectIn();
                connected = true;
                Logger::info("Scan for incoming aprs-msgs from ",
                             Configuration::global_aprsc_host);
            }
            catch (const ConnectionException& ce)
            {
                Logger::error("Failed to connect to APRS input: ", ce.what());
                std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
            }
        }
        if (aprs_con.readLineIn() == CI_MSG_READ_ERR)
        {
            connected = false;
        }
        else
        {
            parser.unpack(std::ref(aprs_con.getResponse()), std::ref(ac_cont));
        }
    }
}

void VFRB::exit_signal_handler(int sig)
{
    global_run_status = false;
    ConnectIn shut("localhost", Configuration::global_out_port);
    try
    {
        shut.setupConnectIn();
        Logger::info("Init shutdown-connection on localhost:",
                     std::to_string(Configuration::global_out_port));
        shut.connectIn();
    }
    catch (const ConnectionException& ce)
    {
        Logger::error("Cannot shutdown nmea-out -> terminate program");
        std::terminate();
    }
}
