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
#include <boost/chrono.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>
#include <csignal>
#include <exception>
#include <string>
#include <utility>

#include "config/Configuration.h"
#include "data/AircraftData.h"
#include "data/AtmosphereData.h"
#include "data/GpsData.h"
#include "data/object/Atmosphere.h"
#include "data/object/Position.h"
#include "data/WindData.h"
#include "feed/AprscFeed.h"
#include "feed/GpsFeed.h"
#include "feed/SbsFeed.h"
#include "feed/SensorFeed.h"
#include "Logger.h"

using namespace data;

#define SYNC_TIME (1)

std::atomic<bool> VFRB::global_run_status(true);

VFRB::VFRB(const config::Configuration& config)
    : mpAircraftData(new AircraftData(config.getMaxDistance())),
      mpAtmosphereData(new AtmosphereData(object::Atmosphere(config.getAtmPressure()))),

      mpGpsData(new GpsData(object::ExtGpsPosition(
          {config.getLatitude(), config.getLongitude(), config.getAltitude()},
          config.getGeoid()))),
      mpWindData(new WindData()),
      mServer(config.getServerPort())
{
    registerFeeds(config);
}

VFRB::~VFRB() noexcept
{}

// only start threads with components here
void VFRB::run() noexcept
{
    Logger::info({"(VFRB) startup"});
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

    signal_set.async_wait([this](const boost::system::error_code&, const int) {
        Logger::info({"(VFRB) caught signal: ", "shutdown"});
        global_run_status = false;
    });

    boost::thread signal_thread([&io_service]() { io_service.run(); });

    // init server and run handler
    boost::thread server_thread([this, &signal_set]() {
        Logger::info({"(Server) start server."});
        mServer.run(signal_set);
        global_run_status = false;
    });

    // init input threads
    boost::thread_group feed_threads;
    for(const auto& it : mFeeds)
    {
        feed_threads.create_thread([&]() {
            Logger::info({"(VFRB) run feed: ", it->getName()});
            it->run(signal_set);
        });
    }
    // mFeeds.clear();

    while(global_run_status)
    {
        try
        {
            // write Aircrafts to clients
            mpAircraftData->processAircrafts(mpGpsData->getGpsPosition(),
                                             mpAtmosphereData->getAtmPress());
            mServer.writeToAll(mpAircraftData->getSerialized());

            // write GPS position to clients
            mServer.writeToAll(mpGpsData->getSerialized());

            // write climate info to clients
            mServer.writeToAll(mpAtmosphereData->getSerialized()
                               + mpWindData->getSerialized());

            // synchronise cycles to ~SYNC_TIME sec
            boost::this_thread::sleep_for(boost::chrono::seconds(SYNC_TIME));
        }
        catch(const std::exception& e)
        {
            Logger::error({"(VFRB) error: ", e.what()});
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

    Logger::info({"EXITING / runtime: ", time_str});
}

void VFRB::registerFeeds(const config::Configuration& crConfig)
{
    for(const auto& feed : crConfig.getFeedMapping())
    {
        try
        {
            if(feed.first.find(SECT_KEY_APRSC) != std::string::npos)
            {
                mFeeds.push_back(std::shared_ptr<feed::Feed>(new feed::AprscFeed(
                    feed.first, feed.second, mpAircraftData, crConfig.getMaxHeight())));
            }
            else if(feed.first.find(SECT_KEY_SBS) != std::string::npos)
            {
                mFeeds.push_back(std::shared_ptr<feed::Feed>(new feed::SbsFeed(
                    feed.first, feed.second, mpAircraftData, crConfig.getMaxHeight())));
            }
            else if(feed.first.find(SECT_KEY_GPS) != std::string::npos)
            {
                mFeeds.push_back(std::shared_ptr<feed::Feed>(new feed::GpsFeed(
                    feed.first, feed.second, mpGpsData, crConfig.isGndModeEnabled())));
            }
            else if(feed.first.find(SECT_KEY_SENS) != std::string::npos)
            {
                mFeeds.push_back(std::shared_ptr<feed::Feed>(new feed::SensorFeed(
                    feed.first, feed.second, mpWindData, mpAtmosphereData)));
            }
            else
            {
                Logger::warn(
                    {"(Config) create feed ", feed.first,
                     ": No keywords found; be sure feed names contain one of " SECT_KEY_APRSC
                     ", " SECT_KEY_SBS ", " SECT_KEY_SENS ", " SECT_KEY_GPS});
            }
        }
        catch(const std::exception& e)
        {
            Logger::warn({"(Config) create feed ", feed.first, ": ", e.what()});
        }
    }
}
