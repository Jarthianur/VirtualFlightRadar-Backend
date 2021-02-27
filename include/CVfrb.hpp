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

#pragma once

#include <atomic>
#include <chrono>

#include "server/CServer.hpp"
#include "server/net/impl/CAcceptorAsio.hpp"
#include "server/net/impl/CSocketAsio.hpp"

#include "Types.hpp"

namespace vfrb::config
{
class CConfiguration;
}  // namespace vfrb::config
namespace vfrb::data
{
class CAircraftData;
class CAtmosphereData;
class CGpsData;
class CWindData;
}  // namespace vfrb::data
namespace vfrb::feed
{
class IFeed;
}  // namespace vfrb::feed

namespace vfrb
{
class CVfrb
{
    Shared<data::CAircraftData>               m_aircraftData;
    Shared<data::CAtmosphereData>             m_atmosphereData;
    Shared<data::CGpsData>                    m_gpsData;
    Shared<data::CWindData>                   m_windData;
    server::CServer<server::net::CSocketAsio> m_server;
    List<Shared<feed::IFeed>>                 m_feeds;
    std::atomic<bool>                         m_running;

    void
    createFeeds(Shared<config::CConfiguration> conf_);

    void
    serve();

    auto static duration(std::chrono::steady_clock::time_point start_) -> String;

public:
    explicit CVfrb(Shared<config::CConfiguration> conf_);

    void
    Run() noexcept;
};
}  // namespace vfrb
