/*
 Copyright_License {

 Copyright (C) 2016 VirtualFlightRadar-Backend
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

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <exception>
#include <functional>
#include <string>

#include "../config/Configuration.h"
#include "../data/AircraftContainer.h"
#include "../data/GPSData.h"
#include "../data/SensorData.h"
#include "../tcp/client/APRSCClient.h"
#include "../tcp/client/SBSClient.h"
#include "../tcp/client/SensorClient.h"
#include "../tcp/server/NMEAServer.h"
#include "../util/Logger.h"

#define SYNC_TIME (1)

std::atomic<bool> VFRB::global_run_status(true);
AircraftContainer VFRB::msAcCont;
SensorData VFRB::msSensorData;
GPSData VFRB::msGPSdata;

VFRB::VFRB()
{
}

VFRB::~VFRB() noexcept
{
}

void VFRB::run() noexcept
{
    Logger::info("(VFRB) startup");
    //store start time
    boost::chrono::steady_clock::time_point start = boost::chrono::steady_clock::now();

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

    boost::thread signal_thread([&io_service]()
    {
        io_service.run();
    });

    // init server and run handler
    NMEAServer server(signal_set, Configuration::global_server_port);
    boost::thread server_thread(boost::bind(&VFRB::handleNMAEServer, std::ref(server)));

    //init input threads
    boost::thread_group threads;
    for (auto it = Configuration::global_feeds.begin();
            it != Configuration::global_feeds.end(); ++it)
    {
        threads.create_thread(
                boost::bind(&VFRB::handleInputFeed, std::ref(signal_set), std::ref(*it)));
    }

    while (global_run_status)
    {
        try
        {
            //write Aircrafts to clients
            std::string str = msAcCont.processAircrafts();
            if (str.length() > 0)
            {
                server.writeToAll(str);
            }

            //write GPS position to clients
            server.writeToAll(msGPSdata.getGPSstr());

            // write wind info to clients
            str = msSensorData.getWVstr();
            if (str.length() > 0)
            {
                server.writeToAll(str);
            }

            //synchronise cycles to ~SYNC_TIME sec
            boost::this_thread::sleep_for(boost::chrono::seconds(SYNC_TIME));
        }
        catch (const std::exception& e)
        {
            Logger::error("(VFRB) error: ", e.what());
            global_run_status = false;
        }
        catch (...)
        {
            Logger::error("(VFRB) error");
            global_run_status = false;
        }
    }

    // exit sequence, join threads
    server_thread.join();
    threads.join_all();
    signal_thread.join();

    //eval end time
    boost::chrono::steady_clock::time_point end = boost::chrono::steady_clock::now();
    boost::chrono::minutes runtime = boost::chrono::duration_cast<boost::chrono::minutes>(
            end - start);
    std::string time_str(std::to_string(runtime.count() / 60 / 24));
    time_str += " days, ";
    time_str += std::to_string(runtime.count() / 60);
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

void VFRB::handleInputFeed(boost::asio::signal_set& sigset, Feed& feed)
{
    Logger::info("(VFRB) run feed: ", feed.mName);
    feed.run(sigset);
}

void VFRB::handleSignals(const boost::system::error_code& ec, const int sig)
{
    Logger::info("(VFRB) caught signal: ", "shutdown");
    global_run_status = false;
}
