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
bool VFRB::global_ogn_enabled = false;
bool VFRB::global_adsb_enabled = false;
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

    if (Configuration::global_ogn_host.compare("nA") != 0 || Configuration::global_ogn_host.length()
            == 0)
    {
        global_ogn_enabled = true;
    }

    if (Configuration::global_adsb_host.compare("nA") != 0 || Configuration::global_adsb_host.length()
            == 0)
    {
        global_adsb_enabled = true;
    }

    if (Configuration::global_weather_feed_host.compare("nA") != 0 || Configuration::global_weather_feed_host.length()
            == 0)
    {
        global_weather_feed_enabled = true;
    }

    if (signal((int) SIGINT, VFRB::exit_signal_handler) == SIG_ERR)
    {
        Logger::error("Failed to register signal: ", "SIGINT");
        global_run_status = false;
        return;
    }

    std::thread adsb_in_thread(handle_adsb_in, std::ref(ac_cont));
    std::thread ogn_in_thread(handle_ogn_in, std::ref(ac_cont));
    std::thread con_out_thread(handle_con_out, std::ref(out_con));
    std::thread weather_feed_thread(handle_weather_feed, std::ref(weather_feed));

    std::string str;

    while (global_run_status)
    {
        try
        {
            ac_cont.invalidateAircrafts();
            for (unsigned int i = 0; i < ac_cont.getContSize(); ++i)
            {
                ac_cont.processAircraft(i, std::ref(str));
                if (str.length() > 0)
                {
                    out_con.sendMsgOut(std::ref(str));
                }
            }

            ac_proc.gpsfix(std::ref(str));
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
                    weather_feed.getNMEA(std::ref(str));
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
        adsb_in_thread.join();
        ogn_in_thread.join();
        con_out_thread.join();
        weather_feed_thread.join();
    }
    catch (const std::system_error& se)
    {
        Logger::error("while joining threads: ", se.what());
        std::terminate();
    }
    Logger::info("EXITING PROGRAM", "");
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
    while (global_run_status)
    {
        out_con.connectClient();
    }
}

void VFRB::handle_weather_feed(WeatherFeed& weather)
{
    if (!global_weather_feed_enabled)
    {
        Logger::warn("Weather feed not enabled -> ", "no wind,pressure,temp");
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

void VFRB::handle_adsb_in(AircraftContainer& ac_cont)
{
    if (!global_adsb_enabled)
    {
        Logger::warn("ADSB receiver not enabled -> ", "no transponder targets");
        return;
    }
    ParserSBS parser;
    ConnectIn adsb_con(std::ref(Configuration::global_adsb_host),
                       Configuration::global_adsb_port);
    bool connected = false;

    while (global_adsb_enabled && global_run_status)
    {
        while (!connected && global_run_status)
        {
            adsb_con.close();
            try
            {
                adsb_con.setupConnectIn();
            }
            catch (const ConnectionException& ce)
            {
                Logger::error("Failed to setup SBS input connection: ", ce.what());
                global_adsb_enabled = false;
                return;
            }
            try
            {
                adsb_con.connectIn();
                connected = true;
                Logger::info("Scan for incoming sbs-msgs from ",
                             Configuration::global_adsb_host);
            }
            catch (const ConnectionException& ce)
            {
                Logger::error("Failed to connect to SBS input: ", ce.what());
                std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
            }
        }
        if (adsb_con.readLineIn() == CI_MSG_READ_ERR)
        {
            connected = false;
        }
        //need msg3 only
        else if (adsb_con.getResponse().at(4) == '3')
        {
            parser.unpack(std::ref(adsb_con.getResponse()), std::ref(ac_cont));
        }
    }
}

void VFRB::handle_ogn_in(AircraftContainer& ac_cont)
{
    if (!global_ogn_enabled)
    {
        Logger::warn("APRSC not enabled -> ", "no FLARM targets");
        return;
    }
    ParserAPRS parser;
    ConnectInExt ogn_con(std::ref(Configuration::global_ogn_host),
                         Configuration::global_ogn_port,
                         std::ref(Configuration::global_login_str));
    bool connected = false;

    while (global_ogn_enabled && global_run_status)
    {
        while (!connected && global_run_status)
        {
            ogn_con.close();
            try
            {
                ogn_con.setupConnectIn();
            }
            catch (const ConnectionException& ce)
            {
                Logger::error("Failed to setup APRS input connection: ", ce.what());
                global_ogn_enabled = false;
                return;
            }
            try
            {
                ogn_con.connectIn();
                connected = true;
                Logger::info("Scan for incoming aprs-msgs from ",
                             Configuration::global_ogn_host);
            }
            catch (const ConnectionException& ce)
            {
                Logger::error("Failed to connect to APRS input: ", ce.what());
                std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
            }
        }
        if (ogn_con.readLineIn() == CI_MSG_READ_ERR)
        {
            connected = false;
        }
        else
        {
            parser.unpack(std::ref(ogn_con.getResponse()), std::ref(ac_cont));
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
        shut.connectIn();
    }
    catch (const ConnectionException& ce)
    {
        Logger::error("Cannot shutdown: ", "nmea out");
    }
}
