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

#include "../vfrb/VFRB.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <csignal>
#include <exception>
#include <functional>
#include <string>

#include "../aircraft/AircraftContainer.h"
#include "../aircraft/AircraftProcessor.h"
#include "../io/client/APRSCClient.h"
#include "../io/client/SBSClient.h"
#include "../io/client/WindClient.h"
#include "../io/logger/Logger.h"
#include "../io/server/NMEAServer.h"
#include "../util/ClimateData.h"
#include "../util/Configuration.h"

bool VFRB::global_climate_enabled = false;
bool VFRB::global_aprsc_enabled = false;
bool VFRB::global_sbs_enabled = false;
bool VFRB::global_run_status = true;
AircraftContainer  VFRB::ac_cont;
ClimateData VFRB::climate_data;

VFRB::VFRB()
{
}

VFRB::~VFRB()
{
}

void VFRB::run()
{
    Logger::info("(VFRB) startup");
    //store start time
    boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();

    // eval config
    if (Configuration::global_aprsc_host.compare("nA") != 0 || Configuration::global_aprsc_host.length()
            == 0)
    {
        global_aprsc_enabled = true;
    }
    else
    {
        Logger::warn("(VFRB) APRSC not enabled: no FLARM targets");
    }

    if (Configuration::global_sbs_host.compare("nA") != 0 || Configuration::global_sbs_host.length()
            == 0)
    {
        global_sbs_enabled = true;
    }
    else
    {
        Logger::warn("(VFRB) SBS input not enabled: no transponder targets");
    }

    if (Configuration::global_climate_host.compare("nA") != 0 || Configuration::global_climate_host.length()
            == 0)
    {
        global_climate_enabled = true;
    }
    else
    {
        Logger::warn("(VFRB) climate input not enabled: no wind,pressure,temp");
    }

    //create ac proc for gpsfix
    AircraftProcessor ac_proc(Configuration::base_latitude, Configuration::base_longitude,
                              Configuration::base_altitude, Configuration::base_geoid);

    // register signals and run handler
    boost::asio::io_service io_service;
    boost::asio::signal_set signal_set(io_service);

    signal_set.add(SIGINT);
    signal_set.add(SIGTERM);
#if defined(SIGQUIT)
    signal_set.add(SIGQUIT);
#endif // defined(SIGQUIT)

    signal_set.async_wait(
            boost::bind(&VFRB::handleSignals, boost::asio::placeholders::error,
                        boost::asio::placeholders::signal_number));

    boost::thread signal_thread([&io_service]() {
        io_service.run();
    });

    // init server and run handler
    NMEAServer server(signal_set, Configuration::global_server_port);
    boost::thread server_thread(boost::bind(&VFRB::handleNMAEServer, std::ref(server)));

    //init input threads
    boost::thread sbs_input_thread(boost::bind(&VFRB::handleSBSInput, std::ref(signal_set)));
    boost::thread aprs_input_thread(boost::bind(&VFRB::handleAPRSCInput, std::ref(signal_set)));
    boost::thread climate_input_thread(boost::bind(&VFRB::handleClimateInput, std::ref(signal_set)));

    while (global_run_status)
    {
        try
        {
            //write Aircrafts to clients
            std::string str = ac_cont.processAircrafts();
            if (str.length() > 0)
            {
                server.writeToAll(str);
            }

            //write GPS position to clients
            str = ac_proc.gpsfix();
            server.writeToAll(str);

            //write wind to clients
            if (global_climate_enabled && climate_data.isValid())
            {
                str = climate_data.extractWV();
                server.writeToAll(str);
            }

            //synchronise cycles to ~VFRB_SYNC_TIME sec
            boost::this_thread::sleep_for(boost::chrono::seconds(SYNC_TIME));
        }
        catch (const std::exception& e)
        {
            Logger::error("(VFRB) error: ", e.what());
            global_run_status = false;
        }
    }

    // exit sequence, join threads
    server_thread.join();
    sbs_input_thread.join();
    aprs_input_thread.join();
    climate_input_thread.join();
    signal_thread.join();

    //eval end time
    boost::chrono::system_clock::time_point end = boost::chrono::system_clock::now();
    boost::chrono::minutes runtime = boost::chrono::duration_cast < boost::chrono::minutes
            > (end - start);
    std::string time_str(std::to_string(runtime.count()/60/24));
    time_str += " days, ";
    time_str += std::to_string(runtime.count()/60);
    time_str += " hours, ";
    time_str += std::to_string(runtime.count() % 60);
    time_str += " mins";

    Logger::info("EXITING / runtime: ", time_str);
}

void VFRB::handleNMAEServer(NMEAServer& server)
{
    Logger::info("(NMEAServer) startup: localhost ",
                 std::to_string(Configuration::global_server_port));
    server.run();
    global_run_status = false;
}

void VFRB::handleClimateInput(boost::asio::signal_set& sigset)
{
    if (!global_climate_enabled)
    {
        return;
    }
    WindClient client(sigset, Configuration::global_climate_host,
                      Configuration::global_climate_port);
    Logger::info("(WindClient) startup: ", Configuration::global_climate_host);
    client.run();
}

void VFRB::handleSBSInput(boost::asio::signal_set& sigset)
{
    if (!global_sbs_enabled)
    {
        return;
    }
    SBSClient client(sigset, Configuration::global_sbs_host,
                     Configuration::global_sbs_port);
    Logger::info("(SBSClient) startup: ", Configuration::global_sbs_host);
    client.run();
}

void VFRB::handleAPRSCInput(boost::asio::signal_set& sigset)
{
    if (!global_aprsc_enabled)
    {
        return;
    }
    APRSCClient client(sigset, Configuration::global_aprsc_host,
                       Configuration::global_aprsc_port,
                       Configuration::global_aprsc_login);
    Logger::info("(APRSCClient) startup: ", Configuration::global_aprsc_host);
    client.run();
}

void VFRB::handleSignals(const boost::system::error_code& ec, const int sig)
{
    Logger::info("(VFRB) caught signal: ", "shutdown");
    global_run_status = false;
}
