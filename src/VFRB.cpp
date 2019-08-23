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
#include "client/net/impl/ConnectorImplBoost.h"
#include "config/Configuration.h"
#include "data/AircraftData.h"
#include "data/AtmosphereData.h"
#include "data/GpsData.h"
#include "data/WindData.h"
#include "feed/Feed.h"
#include "feed/FeedFactory.h"
#include "object/Atmosphere.h"
#include "object/GpsPosition.h"
#include "util/Logger.hpp"
#include "util/SignalListener.h"

using namespace data;
using namespace object;
using namespace config;

constexpr auto PROCESS_INTERVAL = 1;
constexpr auto LOG_PREFIX       = "(VFRB) ";

static auto const& logger = Logger::instance();

VFRB::VFRB(std::shared_ptr<Configuration> config)
    : m_aircraftData(std::make_shared<AircraftData>(
          [this](const Object& it) {
              if (it.updateAge() < Object::OUTDATED)
              {
                  m_server.send(it.nmea());
              }
          },
          config->maxDistance)),
      m_atmosphereData(
          std::make_shared<AtmosphereData>([this](const Object& it) { m_server.send(it.nmea()); },
                                           object::Atmosphere{0, config->atmPressure})),
      m_gpsData(std::make_shared<GpsData>([this](const Object& it) { m_server.send(it.nmea()); },
                                          config->gpsPosition, config->groundMode)),
      m_windData(std::make_shared<WindData>([this](const Object& it) { m_server.send(it.nmea()); })),
      m_server(config->serverPort),
      m_running(false)
{
    createFeeds(config);
}

void VFRB::run() noexcept
{
    m_running = true;
    logger.info(LOG_PREFIX, "starting...");
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    util::SignalListener                  signals;
    client::ClientManager                 clientManager;

    signals.addHandler([this](const boost::system::error_code&, const int) {
        logger.info(LOG_PREFIX, "caught signal to shutdown...");
        m_running = false;
    });
    for (auto it : m_feeds)
    {
        logger.info(LOG_PREFIX, "run feed: ", it->name);
        try
        {
            clientManager.subscribe(it);
        }
        catch (const std::logic_error& e)
        {
            logger.error(LOG_PREFIX, ": ", e.what());
        }
    }
    m_feeds.clear();

    signals.run();
    m_server.run();
    clientManager.run();
    serve();
    clientManager.stop();
    m_server.stop();
    signals.stop();
    logger.info(LOG_PREFIX, "stopped after ", duration(start));
}

void VFRB::serve()
{
    std::this_thread::sleep_for(std::chrono::seconds(PROCESS_INTERVAL));
    while (m_running)
    {
        try
        {
            m_aircraftData->environment(m_gpsData->location(), m_atmosphereData->atmPressure());
            m_aircraftData->access();
            m_gpsData->access();
            m_atmosphereData->access();
            m_windData->access();
            std::this_thread::sleep_for(std::chrono::seconds(PROCESS_INTERVAL));
        }
        catch (const std::exception& e)
        {
            logger.error(LOG_PREFIX, "fatal: ", e.what());
            m_running = false;
        }
    }
}

void VFRB::createFeeds(std::shared_ptr<Configuration> config)
{
    feed::FeedFactory factory(config, m_aircraftData, m_atmosphereData, m_gpsData, m_windData);
    for (const auto& name : config->feedNames)
    {
        try
        {
            m_feeds.push_back(factory.createFeed(name));
        }
        catch (const std::exception& e)
        {
            logger.warn(LOG_PREFIX, "can not create feed ", name, ": ", e.what());
        }
    }
}

std::string VFRB::duration(std::chrono::steady_clock::time_point start) const
{
    std::chrono::minutes runtime =
        std::chrono::duration_cast<std::chrono::minutes>(std::chrono::steady_clock::now() - start);
    std::int64_t      d = runtime.count() / 60 / 24;
    std::int64_t      h = runtime.count() / 60 - d * 24;
    std::int64_t      m = runtime.count() % 60;
    std::stringstream ss;
    ss << d << "d " << h << ":" << m;
    return ss.str();
}
