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
#include <sstream>

#include "client/ClientManager.h"
#include "client/net/impl/ConnectorImplBoost.h"
#include "concurrent/SignalListener.h"
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

using namespace vfrb::data;
using namespace vfrb::object;
using namespace vfrb::config;

namespace vfrb
{
constexpr auto PROCESS_INTERVAL = 1;
constexpr auto LOG_PREFIX       = "(VFRB) ";

static auto const& logger = CLogger::Instance();

CVfrb::CVfrb(SPtr<CConfiguration> config)
    : m_aircraftData(std::make_shared<CAircraftData>(
          [this](SAccessor const& it) {
              if (it.Obj.UpdateAge() < CObject::OUTDATED)
              {
                  m_server.send(it.Nmea);
              }
          },
          config->maxDistance)),
      m_atmosphereData(
          std::make_shared<CAtmosphereData>([this](SAccessor const& it) { m_server.send(it.Nmea); },
                                           object::CAtmosphere{0, config->atmPressure})),
      m_gpsData(std::make_shared<CGpsData>([this](SAccessor const& it) { m_server.send(it.Nmea); },
                                          config->gpsPosition, config->groundMode)),
      m_windData(std::make_shared<CWindData>([this](SAccessor const& it) { m_server.send(it.Nmea); })),
      m_server(std::get<0>(config->serverConfig), std::get<1>(config->serverConfig)),
      m_running(false)
{
    createFeeds(config);
}

void CVfrb::Run() noexcept
{
    m_running = true;
    logger.info(LOG_PREFIX, "starting...");
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    concurrent::CSignalListener            signals;
    client::CClientManager                 clientManager;

    signals.addHandler([this](boost::system::error_code const&, [[maybe_unused]] const int) {
        logger.info(LOG_PREFIX, "caught signal to shutdown...");
        m_running = false;
    });
    for (auto it : m_feeds)
    {
        logger.info(LOG_PREFIX, "run feed: ", it->name());
        try
        {
            clientManager.subscribe(it);
        }
        catch (client::error::CFeedSubscriptionError const& e)
        {
            logger.error(LOG_PREFIX, ": ", e.what());
        }
    }
    m_feeds.clear();

    signals.Run();
    m_server.run();
    clientManager.run();
    serve();
    clientManager.stop();
    m_server.stop();
    signals.stop();
    logger.info(LOG_PREFIX, "stopped after ", duration(start));
}

void CVfrb::serve()
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
        catch (error::IError const& e)
        {
            logger.error(LOG_PREFIX, "fatal: ", e.what());
            m_running = false;
        }
    }
}

void CVfrb::createFeeds(SPtr<CConfiguration> config)
{
    feed::FeedFactory factory(config, m_aircraftData, m_atmosphereData, m_gpsData, m_windData);
    for (auto const& name : config->feedNames)
    {
        try
        {
            m_feeds.push_back(factory.createFeed(name));
        }
        catch (feed::error::FeedCreationError const& e)
        {
            logger.warn(LOG_PREFIX, "can not create feed ", name, ": ", e.what());
        }
    }
}

Str CVfrb::duration(std::chrono::steady_clock::time_point start) const
{
    std::chrono::minutes runtime =
        std::chrono::duration_cast<std::chrono::minutes>(std::chrono::steady_clock::now() - start);
    s64               d = runtime.count() / 60 / 24;
    s64               h = runtime.count() / 60 - d * 24;
    s64               m = runtime.count() % 60;
    std::stringstream ss;
    ss << d << "d " << (h < 10 ? "0" : "") << h << ":" << (m < 10 ? "0" : "") << m;
    return ss.str();
}
}  // namespace vfrb
