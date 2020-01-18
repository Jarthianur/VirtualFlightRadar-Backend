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

#include "Vfrb.h"

#include <csignal>
#include <sstream>

#include "client/ClientManager.h"
#include "client/net/impl/ConnectorBoost.h"
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

CVfrb::CVfrb(SPtr<CConfiguration> conf_)
    : m_aircraftData(std::make_shared<CAircraftData>(
          [this](SAccessor const& it) {
              if (it.Obj.UpdateAge() < CObject::OUTDATED)
              {
                  m_server.Send(it.Nmea);
              }
          },
          conf_->MaxDistance)),
      m_atmosphereData(
          std::make_shared<CAtmosphereData>([this](SAccessor const& it) { m_server.Send(it.Nmea); },
                                            object::CAtmosphere{0, conf_->AtmPressure})),
      m_gpsData(std::make_shared<CGpsData>([this](SAccessor const& it) { m_server.Send(it.Nmea); },
                                           conf_->GpsPosition, conf_->GroundMode)),
      m_windData(std::make_shared<CWindData>([this](SAccessor const& it) { m_server.Send(it.Nmea); })),
      m_server(std::get<0>(conf_->ServerConfig), std::get<1>(conf_->ServerConfig)),
      m_running(false)
{
    createFeeds(conf_);
}

void CVfrb::Run() noexcept
{
    m_running = true;
    logger.Info(LOG_PREFIX, "starting...");
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    concurrent::CSignalListener           signals;
    client::CClientManager                clientManager;

    signals.AddHandler([this](boost::system::error_code const&, [[maybe_unused]] const int) {
        logger.Info(LOG_PREFIX, "caught signal to shutdown...");
        m_running = false;
    });
    for (auto it : m_feeds)
    {
        logger.Info(LOG_PREFIX, "run feed: ", it->Name());
        try
        {
            clientManager.Subscribe(it);
        }
        catch (client::error::CFeedSubscriptionError const& e)
        {
            logger.Error(LOG_PREFIX, ": ", e.Message());
        }
    }
    m_feeds.clear();

    signals.Run();
    m_server.Run();
    clientManager.Run();
    serve();
    clientManager.Stop();
    m_server.Stop();
    signals.Stop();
    logger.Info(LOG_PREFIX, "stopped after ", duration(start));
}

void CVfrb::serve()
{
    std::this_thread::sleep_for(std::chrono::seconds(PROCESS_INTERVAL));
    while (m_running)
    {
        try
        {
            m_aircraftData->Environment(m_gpsData->Location(), m_atmosphereData->Pressure());
            m_aircraftData->Access();
            m_gpsData->Access();
            m_atmosphereData->Access();
            m_windData->Access();
            std::this_thread::sleep_for(std::chrono::seconds(PROCESS_INTERVAL));
        }
        catch (error::IError const& e)
        {
            logger.Error(LOG_PREFIX, "fatal: ", e.Message());
            m_running = false;
        }
    }
}

void CVfrb::createFeeds(SPtr<CConfiguration> conf_)
{
    feed::CFeedFactory factory(conf_, m_aircraftData, m_atmosphereData, m_gpsData, m_windData);
    for (auto const& name : conf_->FeedNames)
    {
        try
        {
            m_feeds.push_back(factory.createFeed(name));
        }
        catch (feed::error::CFeedCreationError const& e)
        {
            logger.Warn(LOG_PREFIX, "can not create feed ", name, ": ", e.Message());
        }
    }
}

Str CVfrb::duration(std::chrono::steady_clock::time_point start_) const
{
    std::chrono::minutes runtime =
        std::chrono::duration_cast<std::chrono::minutes>(std::chrono::steady_clock::now() - start_);
    s64               d = runtime.count() / 60 / 24;
    s64               h = runtime.count() / 60 - d * 24;
    s64               m = runtime.count() % 60;
    std::stringstream ss;
    ss << d << "d " << (h < 10 ? "0" : "") << h << ":" << (m < 10 ? "0" : "") << m;
    return ss.str();
}
}  // namespace vfrb
