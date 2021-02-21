/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "CVfrb.hpp"

#include <csignal>
#include <sstream>

#include "client/CClientManager.hpp"
#include "client/net/impl/CConnectorBoost.hpp"
#include "concurrent/CSignalListener.hpp"
#include "config/CConfiguration.hpp"
#include "data/CAircraftData.hpp"
#include "data/CAtmosphereData.hpp"
#include "data/CGpsData.hpp"
#include "data/CWindData.hpp"
#include "data/IData.hpp"
#include "feed/CFeedFactory.hpp"
#include "feed/IFeed.hpp"
#include "object/CAtmosphere.hpp"
#include "object/CGpsPosition.hpp"
#include "object/CObject.hpp"
#include "util/ClassUtils.hpp"

#include "CLogger.hpp"

using vfrb::data::CAircraftData;
using vfrb::data::CAtmosphereData;
using vfrb::data::CWindData;
using vfrb::data::CGpsData;
using vfrb::data::SAccessor;
using vfrb::object::CObject;
using vfrb::config::CConfiguration;

namespace vfrb
{
CTCONST PROCESS_INTERVAL = 1;
CTCONST LOG_PREFIX       = "(VFRB) ";

static auto const& logger = CLogger::Instance();

CVfrb::CVfrb(SPtr<CConfiguration> conf_)
    : m_aircraftData(std::make_shared<CAircraftData>(
          [this](SAccessor const& it) {
              if (it.Obj.UpdateAge() <= CObject::OUTDATED) {
                  m_server.Send(it.Nmea);
              }
          },
          conf_->MaxDistance)),
      m_atmosphereData(
          std::make_shared<CAtmosphereData>([this](SAccessor const& it) { m_server.Send(it.Nmea); },
                                            object::CAtmosphere{0, conf_->AtmPressure, ""})),
      m_gpsData(std::make_shared<CGpsData>([this](SAccessor const& it) { m_server.Send(it.Nmea); },
                                           conf_->GpsPosition, conf_->GroundMode)),
      m_windData(std::make_shared<CWindData>([this](SAccessor const& it) { m_server.Send(it.Nmea); })),
      m_server(std::get<0>(conf_->ServerConfig), std::get<1>(conf_->ServerConfig)),
      m_running(false) {
    createFeeds(conf_);
}

void
CVfrb::Run() noexcept {
    m_running = true;
    logger.Info(LOG_PREFIX, "starting...");
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    concurrent::CSignalListener           signals;
    client::CClientManager                clientManager;

    signals.AddHandler([this](boost::system::error_code const&, [[maybe_unused]] const int) {
        logger.Info(LOG_PREFIX, "caught signal to shutdown...");
        m_running = false;
    });
    for (auto it : m_feeds) {
        logger.Info(LOG_PREFIX, "run feed: ", it->Name());
        try {
            clientManager.Subscribe(it);
        } catch (client::error::CFeedSubscriptionError const& e) {
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

void
CVfrb::serve() {
    std::this_thread::sleep_for(std::chrono::seconds(PROCESS_INTERVAL));
    while (m_running) {
        try {
            m_aircraftData->Environment(m_gpsData->Location(), m_atmosphereData->Pressure());
            m_aircraftData->Access();
            m_gpsData->Access();
            m_atmosphereData->Access();
            m_windData->Access();
            std::this_thread::sleep_for(std::chrono::seconds(PROCESS_INTERVAL));
        } catch (error::IError const& e) {
            logger.Error(LOG_PREFIX, "fatal: ", e.Message());
            m_running = false;
        }
    }
}

void
CVfrb::createFeeds(SPtr<CConfiguration> conf_) {
    feed::CFeedFactory factory(conf_, m_aircraftData, m_atmosphereData, m_gpsData, m_windData);
    for (auto const& name : conf_->FeedNames) {
        try {
            m_feeds.push_back(factory.createFeed(name));
        } catch (feed::error::CFeedCreationError const& e) {
            logger.Warn(LOG_PREFIX, "can not create feed ", name, ": ", e.Message());
        }
    }
}

auto
CVfrb::duration(std::chrono::steady_clock::time_point start_) -> String {
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
