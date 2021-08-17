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
#include "client/net/impl/CConnectorAsio.hpp"
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
using vfrb::config::CConfiguration;
using vfrb::str_util::StringInserter;

namespace vfrb
{
CTCONST PROCESS_INTERVAL = 1000;
CTCONST LOG_PREFIX       = "(VFRB) ";

static auto const& logger = CLogger::Instance();

CVfrb::CVfrb(Shared<CConfiguration> conf_)
    : m_aircraftData(AllocShared<CAircraftData>(conf_->MaxDistance)),
      m_atmosphereData(AllocShared<CAtmosphereData>(object::CAtmosphere{0, conf_->AtmPressure, ""})),
      m_gpsData(AllocShared<CGpsData>(conf_->GpsPosition, conf_->GroundMode)),
      m_windData(AllocShared<CWindData>()),
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

    signals.AddHandler([this](asio::error_code const&, [[maybe_unused]] const int) {
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
    std::this_thread::sleep_for(std::chrono::milliseconds(PROCESS_INTERVAL));
    String data;
    data.reserve(2048);  // initially allocate 2 KiB
    usize priorAircraftCount{0};
    while (m_running) {
        try {
            // process, gather and send all data
            auto before = std::chrono::steady_clock::now();
            m_aircraftData->Environment(m_gpsData->Location(), m_atmosphereData->Pressure());
            m_gpsData->CollectInto(StringInserter(data));
            m_atmosphereData->CollectInto(StringInserter(data));
            m_windData->CollectInto(StringInserter(data));
            m_aircraftData->CollectInto(StringInserter(data));
            m_server.Send(data);
            // free unused memory for data at some point
            auto newAircraftCount = m_aircraftData->Size();
            if (newAircraftCount < priorAircraftCount * .8) {
                data.shrink_to_fit();
                priorAircraftCount = newAircraftCount;
            } else if (newAircraftCount > priorAircraftCount) {
                priorAircraftCount = newAircraftCount;
            }
            data.clear();
            // wait remaining time of interval
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - before);
            if (diff.count() < PROCESS_INTERVAL) {
                std::this_thread::sleep_for(std::chrono::milliseconds(PROCESS_INTERVAL) - diff);
            } else {
                logger.Warn(LOG_PREFIX, "Could not keep up, processing takes longer than 1 second");
            }
        } catch (error::IError const& e) {
            logger.Error(LOG_PREFIX, "fatal: ", e.Message());
            m_running = false;
        }
    }
}

void
CVfrb::createFeeds(Shared<CConfiguration> conf_) {
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
    i64               d = runtime.count() / 60 / 24;
    i64               h = runtime.count() / 60 - d * 24;
    i64               m = runtime.count() % 60;
    std::stringstream ss;
    ss << d << "d " << (h < 10 ? "0" : "") << h << ":" << (m < 10 ? "0" : "") << m;
    return ss.str();
}
}  // namespace vfrb
