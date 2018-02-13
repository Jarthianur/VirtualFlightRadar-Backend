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

#include <exception>
#include <functional>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/move/move.hpp>
#include <boost/thread.hpp>

#include "config/Configuration.h"
#include "data/AtmosphereData.h"
#include "data/GpsData.h"
#include "data/WindData.h"
#include "feed/Feed.h"
#include "network/client/AprscClient.h"
#include "network/client/SbsClient.h"
#include "network/client/SensorClient.h"

#include "util/Logger.h"
#include "util/Position.h"
#include "util/Sensor.h"

using namespace util;

#define SYNC_TIME (1)

std::atomic<bool> VFRB::global_run_status(true);

VFRB::VFRB(const config::Configuration& config)
    :mServer(config.getSServerPort())
{
    //TODO create config object here and init all components

}

VFRB::~VFRB() noexcept
{}

// only start threads with components here
void VFRB::run() noexcept
{
    Logger::info("(VFRB) startup");
    // store start time
    boost::chrono::steady_clock::time_point start = boost::chrono::steady_clock::now();

    // register signals and run handler
    boost::asio::io_service io_service;
    boost::asio::signal_set signal_set(io_service);

    signal_set.add(SIGINT);
    signal_set.add(SIGTERM);
#if defined(SIGQUIT)
    signal_set.add(SIGQUIT);
#endif  // defined(SIGQUIT)

    signal_set.async_wait(boost::bind(&VFRB::handleSignals,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::signal_number));

    boost::thread signal_thread([&io_service]() { io_service.run(); });

    // init server and run handler
    network::server::Server server(signal_set, config::Configuration::sServerPort);
    boost::thread server_thread(boost::bind(&VFRB::handleServer, std::ref(server)));

    // init input threads
    boost::thread_group feed_threads;
    for(auto it = config::Configuration::sRegisteredFeeds.begin();
        it != config::Configuration::sRegisteredFeeds.end(); ++it)
    {
        feed_threads.create_thread(
            boost::bind(&VFRB::handleFeed, std::ref(signal_set), *it));
    }
    config::Configuration::sRegisteredFeeds.clear();

    while(global_run_status)
    {
        try
        {
            // write Aircrafts to clients
            std::string str = msAcCont.processAircrafts(
                {msGpsData.getBaseLat(), msGpsData.getBaseLong(), msGpsData.getBaseAlt()},
                msAtmosData.getAtmPress());

            if(str.length() > 0)
            {
                server.writeToAll(str);
            }

            // write GPS position to clients
            server.writeToAll(msGpsData.getGpsStr());

            // write climate info to clients
            str = msAtmosData.getMdaStr() + msWindData.getMwvStr();
            if(str.length() > 0)
            {
                server.writeToAll(str);
            }

            // synchronise cycles to ~SYNC_TIME sec
            boost::this_thread::sleep_for(boost::chrono::seconds(SYNC_TIME));
        }
        catch(const std::exception& e)
        {
            Logger::error("(VFRB) error: ", e.what());
            global_run_status = false;
        }
    }

    // exit sequence, join threads
    server_thread.join();
    feed_threads.join_all();
    signal_thread.join();

    // eval end time
    boost::chrono::steady_clock::time_point end = boost::chrono::steady_clock::now();
    boost::chrono::minutes runtime
        = boost::chrono::duration_cast<boost::chrono::minutes>(end - start);
    std::string time_str(std::to_string(runtime.count() / 60 / 24));
    time_str += " days, ";
    time_str += std::to_string(runtime.count() / 60);
    time_str += " hours, ";
    time_str += std::to_string(runtime.count() % 60);
    time_str += " mins";

    Logger::info("EXITING / runtime: ", time_str);
}

std::size_t VFRB::registerFeeds(const PropertyMap& crProperties)
{
    std::vector<std::string> feeds
        = resolveFeeds(crProperties.getProperty(SECT_KEY_GENERAL, KV_KEY_FEEDS));
    std::vector<std::function<bool(const std::string&, const PropertyMap&)>> creators;
    creators.push_back(registerCreator<feed::AprscFeed>(SECT_KEY_APRSC));
    creators.push_back(registerCreator<feed::SbsFeed>(SECT_KEY_SBS));
    creators.push_back(registerCreator<feed::SensorFeed>(SECT_KEY_SENS));
    creators.push_back(registerCreator<feed::GpsFeed>(SECT_KEY_GPS));

    for(const auto& feed : feeds)
    {
        bool found = false;
        for(auto& creator : creators)
        {
            try
            {
                if((found = creator(feed, crProperties)))
                {
                    break;
                }
            }
            catch(const std::exception& e)
            {
                Logger::warn("(Config) create feed " + feed + ": ", e.what());
                found = true;
                break;
            }
        }
        if(!found)
        {
            Logger::warn(
                "(Config) create feed " + feed,
                ": No keywords found; be sure feed names contain one of " SECT_KEY_APRSC
                ", " SECT_KEY_SBS ", " SECT_KEY_SENS ", " SECT_KEY_GPS);
        }
    }
    return sRegisteredFeeds.size();
}

void VFRB::handleServer(network::server::Server& r_server)
{
    Logger::info("(Server) startup: localhost ",
                 std::to_string(config::Configuration::sServerPort));
    r_server.run();
    global_run_status = false;
}

void VFRB::handleFeed(boost::asio::signal_set& r_sigset,
                      std::shared_ptr<feed::Feed> p_feed)
{
    Logger::info("(VFRB) run feed: ", p_feed->getName());
    p_feed->run(r_sigset);
}

void VFRB::handleSignals(const boost::system::error_code& cr_ec, const int sig)
{
    Logger::info("(VFRB) caught signal: ", "shutdown");
    global_run_status = false;
}
