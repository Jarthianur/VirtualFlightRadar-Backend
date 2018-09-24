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
#include <thread>

#include "client/ClientManager.h"
#include "client/ConnectorImplBoost.h"
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
#include "Signals.h"

using namespace data;

#define SYNC_TIME 1

VFRB::VFRB(const config::Configuration& config)
    : m_aircraftData(std::make_shared<AircraftData>(config.get_maxDistance())),
      m_atmosphereData(
          std::make_shared<AtmosphereData>(object::Atmosphere(config.get_atmPressure(), 0))),
      m_gpsData(std::make_shared<GpsData>(config.get_position(), config.get_groundMode())),
      m_windData(std::make_shared<WindData>()),
      m_server(config.get_serverPort()),
      m_running(false)
{
    createFeeds(config);
}

VFRB::~VFRB() noexcept
{}

void VFRB::run() noexcept
{
    m_running = true;
    logger.info("(VFRB) startup");
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    Signals signals;
    client::ClientManager clientManager;

    signals.addHandler([this](const boost::system::error_code&, const int) {
        logger.info("(VFRB) caught signal to shutdown ...");
        m_running = false;
    });

    for(const auto& it : m_feeds)
    {
        logger.info("(VFRB) run feed: ", it->get_name());
        clientManager.subscribe(it);
    }
    m_feeds.clear();

    signals.run();
    m_server.run();
    clientManager.run();

    serve();

        clientManager.stop();
    m_server.stop();
        signals.stop();

    logger.info("Stopped after ", get_duration(start));
}

void VFRB::serve()
{
    while(m_running)
    {
        try
        {
            m_aircraftData->processAircrafts(m_gpsData->get_position(),
                                             m_atmosphereData->get_atmPressure());
            m_server.send(m_aircraftData->get_serialized());
            m_server.send(m_gpsData->get_serialized());
            m_server.send(m_atmosphereData->get_serialized());
            m_server.send(m_windData->get_serialized());
            std::this_thread::sleep_for(std::chrono::seconds(SYNC_TIME));
        }
        catch(const std::exception& e)
        {
            logger.error("(VFRB) error: ", e.what());
            m_running = false;
        }
    }
}

void VFRB::createFeeds(const config::Configuration& config)
{
    feed::FeedFactory factory(config, m_aircraftData, m_atmosphereData, m_gpsData, m_windData);
    for(const auto& feed : config.get_feedProperties())
    {
        try
        {
            auto optFeedPtr = factory.createFeed(feed.first, feed.second);
            if(optFeedPtr)
            {
                m_feeds.push_back(*optFeedPtr);
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

std::string VFRB::get_duration(std::chrono::steady_clock::time_point start) const
{
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::chrono::minutes runtime = std::chrono::duration_cast<std::chrono::minutes>(end - start);
    std::stringstream ss;
    ss << runtime.count() / 60 / 24 << " days, " << runtime.count() / 60 << " hours, "
       << runtime.count() % 60 << " minutes";
    return ss.str();
}
