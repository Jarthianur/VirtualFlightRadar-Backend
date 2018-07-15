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

#include <csignal>
#include <exception>
#include <sstream>
#include <string>
#include <utility>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>

#include "config/Configuration.h"
#include "data/AircraftData.h"
#include "data/AtmosphereData.h"
#include "data/GpsData.h"
#include "data/WindData.h"
#include "feed/Feed.h"
#include "feed/FeedFactory.h"
#include "object/Atmosphere.h"
#include "object/GpsPosition.h"

#include "Logger.hpp"

using namespace data;

#define SYNC_TIME 1

std::atomic<bool> VFRB::vRunStatus(true);

VFRB::VFRB(const config::Configuration& config)
    : mpAircraftData(std::make_shared<AircraftData>(config.getMaxDistance())),
      mpAtmosphereData(
          std::make_shared<AtmosphereData>(object::Atmosphere(config.getAtmPressure(), 0))),
      mpGpsData(std::make_shared<GpsData>(config.getPosition(), config.getGroundMode())),
      mpWindData(std::make_shared<WindData>()),
      mServer(config.getServerPort())
{
    createFeeds(config);
}

VFRB::~VFRB() noexcept
{}

void VFRB::run() noexcept
{
    Logger::info("(VFRB) startup");
    boost::chrono::steady_clock::time_point start = boost::chrono::steady_clock::now();

    boost::asio::io_service io_service;
    boost::asio::signal_set signal_set(io_service);
    setupSignals(signal_set);

    boost::thread server_thread([this, &signal_set]() {
        Logger::info("(Server) start server");
        mServer.run(signal_set);
        vRunStatus = false;
    });

    for(const auto& it : mFeeds)
    {
        Logger::info("(VFRB) run feed: ", it->getName());
        it->registerToClient(mClientManager);
    }

    mClientManager.run();
    boost::thread signal_thread([&io_service]() { io_service.run(); });
    mFeeds.clear();

    serve();

    mClientManager.stop();
    server_thread.join();
    signal_thread.join();

    Logger::info("Stopped after ", getDuration(start));
}

void VFRB::createFeeds(const config::Configuration& crConfig)
{
    feed::FeedFactory factory(crConfig, mpAircraftData, mpAtmosphereData, mpGpsData, mpWindData);
    for(const auto& feed : crConfig.getFeedMapping())
    {
        try
        {
            auto optFeedPtr = factory.createFeed(feed.first, feed.second);
            if(optFeedPtr)
            {
                mFeeds.push_back(*optFeedPtr);
            }
            else
            {
                Logger::warn(
                    "(VFRB) create feed ", feed.first,
                    ": No keywords found; be sure feed names contain one of " SECT_KEY_APRSC
                    ", " SECT_KEY_SBS ", " SECT_KEY_WIND ", " SECT_KEY_ATMOS ", " SECT_KEY_GPS);
            }
        }
        catch(const std::exception& e)
        {
            Logger::warn("(VFRB) create feed ", feed.first, ": ", e.what());
        }
    }
}

void VFRB::setupSignals(boost::asio::signal_set& rSigSet)
{
    rSigSet.add(SIGINT);
    rSigSet.add(SIGTERM);
#ifdef SIGQUIT
    rSigSet.add(SIGQUIT);
#endif

    rSigSet.async_wait([this](const boost::system::error_code&, const int) {
        Logger::info("(VFRB) caught signal: ", "shutdown");
        vRunStatus = false;
    });
}

void VFRB::serve()
{
    while(vRunStatus)
    {
        try
        {
            mpAircraftData->processAircrafts(mpGpsData->getPosition(),
                                             mpAtmosphereData->getAtmPressure());
            mServer.send(mpAircraftData->getSerialized());
            mServer.send(mpGpsData->getSerialized());
            mServer.send(mpAtmosphereData->getSerialized());
            mServer.send(mpWindData->getSerialized());
            boost::this_thread::sleep_for(boost::chrono::seconds(SYNC_TIME));
        }
        catch(const std::exception& e)
        {
            Logger::error("(VFRB) error: ", e.what());
            vRunStatus = false;
        }
    }
}

std::string VFRB::getDuration(boost::chrono::steady_clock::time_point vStart) const
{
    boost::chrono::steady_clock::time_point end = boost::chrono::steady_clock::now();
    boost::chrono::minutes runtime
        = boost::chrono::duration_cast<boost::chrono::minutes>(end - vStart);
    std::stringstream ss;
    ss << runtime.count() / 60 / 24 << " days, " << runtime.count() / 60 << " hours, "
       << runtime.count() % 60 << " minutes";
    return ss.str();
}
