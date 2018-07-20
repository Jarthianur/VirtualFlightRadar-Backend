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
#include <boost/thread.hpp>

#include "config/Configuration.h"
#include "data/AircraftData.h"
#include "data/AtmosphereData.h"
#include "data/GpsData.h"
#include "data/WindData.h"
#include "feed/Feed.h"
#include "feed/FeedFactory.h"
#include "client/ClientManager.hpp"
#include "client/ConnectorImplBoost.h"
#include "object/Atmosphere.h"
#include "object/GpsPosition.h"
#include "Logger.hpp"
#include "Signals.h"

using namespace data;

#define SYNC_TIME 1

VFRB::VFRB(const config::Configuration& config)
    : mpAircraftData(std::make_shared<AircraftData>(config.getMaxDistance())),
      mpAtmosphereData(
          std::make_shared<AtmosphereData>(object::Atmosphere(config.getAtmPressure(), 0))),
      mpGpsData(std::make_shared<GpsData>(config.getPosition(), config.getGroundMode())),
      mpWindData(std::make_shared<WindData>()),
      mServer(config.getServerPort()),
      mRunStatus(false)
{
    createFeeds(config);
}

VFRB::~VFRB() noexcept
{}

void VFRB::run() noexcept
{
    logger.info("(VFRB) startup");
    boost::chrono::steady_clock::time_point start = boost::chrono::steady_clock::now();
    mRunStatus                                    = true;

    Signals signals;
    client::ClientManager<client::ConnectorImplBoost> clientManager;

    signals.addHandler([this](const boost::system::error_code&, const int) {
        logger.info("(VFRB) caught signal to shutdown ...");
        mRunStatus = false;
    });

    for(const auto& it : mFeeds)
    {
        logger.info("(VFRB) run feed: ", it->getName());
        clientManager.subscribe(it);
    }
    mFeeds.clear();

    signals.run();
    mServer.run();
    clientManager.run();

    serve();

    signals.stop();
    clientManager.stop();
    mServer.stop();

    logger.info("Stopped after ", getDuration(start));
}

void VFRB::serve()
{
    while(mRunStatus)
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
            logger.error("(VFRB) error: ", e.what());
            mRunStatus = false;
        }
    }
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
                logger.warn("(VFRB) create feed ", feed.first,
                            ": No keywords found; be sure feed names contain one of " SECT_KEY_APRSC
                            ", " SECT_KEY_SBS ", " SECT_KEY_WIND ", " SECT_KEY_ATMOS
                            ", " SECT_KEY_GPS);
            }
        }
        catch(const std::exception& e)
        {
            logger.warn("(VFRB) create feed ", feed.first, ": ", e.what());
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
