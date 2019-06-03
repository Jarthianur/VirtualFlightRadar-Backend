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

VFRB::VFRB(std::shared_ptr<Configuration> config)
    : m_aircraftData(std::make_shared<AircraftData>(config->maxDistance)),
      m_atmosphereData(std::make_shared<AtmosphereData>(object::Atmosphere{0, config->atmPressure})),
      m_gpsData(std::make_shared<GpsData>(config->gpsPosition, config->groundMode)),
      m_windData(std::make_shared<WindData>()),
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
    logger.info(LOG_PREFIX, "stopped after ", get_duration(start));
}

void VFRB::serve()
{
    std::this_thread::sleep_for(std::chrono::seconds(PROCESS_INTERVAL));
    while (m_running)
    {
        try
        {
            m_aircraftData->setEnvironment(m_gpsData->getLocation(), m_atmosphereData->getAtmPressure());
            m_aircraftData->access([this](const Object& it) {
                if (it.getUpdateAge() < Object::OUTDATED)
                {
                    m_server.send(it.getNMEA());
                }
            });

            auto fn = [this](const Object& it) { m_server.send(it.getNMEA()); };
            m_gpsData->access(fn);
            m_atmosphereData->access(fn);
            m_windData->access(fn);
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
            auto optFeedPtr = factory.createFeed(name);
            if (optFeedPtr)
            {
                m_feeds.push_back(*optFeedPtr);
            }
            else
            {
                logger.warn(LOG_PREFIX, " can not create feed ", name,
                            ": No keywords found; be sure feed names contain one of ",
                            Configuration::SECT_KEY_APRSC, ", ", Configuration::SECT_KEY_SBS, ", ",
                            Configuration::SECT_KEY_WIND, ", ", Configuration::SECT_KEY_ATMOS, ", ",
                            Configuration::SECT_KEY_GPS);
            }
        }
        catch (const std::exception& e)
        {
            logger.warn(LOG_PREFIX, "can not create feed ", name, ": ", e.what());
        }
    }
}

std::string VFRB::get_duration(std::chrono::steady_clock::time_point start) const
{
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::chrono::minutes runtime              = std::chrono::duration_cast<std::chrono::minutes>(end - start);
    std::stringstream    ss;
    ss << runtime.count() / 60 / 24 << " days, " << runtime.count() / 60 << " hours, " << runtime.count() % 60
       << " minutes";
    return ss.str();
}
