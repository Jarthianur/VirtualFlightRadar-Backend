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
#include <list>

#include "server/CServer.hpp"
#include "server/net/impl/CAcceptorAsio.hpp"
#include "server/net/impl/CSocketAsio.hpp"
#include "util/ClassUtils.hpp"

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
/**
 * @brief Combine all features and is the main entry point for the actual VFR-B.
 */
class CVfrb
{
    SPtr<data::CAircraftData>                 m_aircraftData;    ///< Aircraft container
    SPtr<data::CAtmosphereData>               m_atmosphereData;  ///< Atmospheric data container
    SPtr<data::CGpsData>                      m_gpsData;         ///< GPS data container
    SPtr<data::CWindData>                     m_windData;        ///< Wind data container
    server::CServer<server::net::CSocketAsio> m_server;          ///< Manage clients and sending of data
    List<SPtr<feed::IFeed>>                   m_feeds;           ///< List of all active feeds
    std::atomic<bool>                         m_running;         ///< Atomic run-status

    /**
     * @brief Create all input feeds.
     * @param config The Configuration
     */
    void
    createFeeds(SPtr<config::CConfiguration> conf_);

    /**
     * @brief Serve the data frequently every second.
     */
    void
    serve();

    /**
     * @brief Get the duration from given start value as formatted string.
     * @param start The start value
     * @return the duration string
     */
    auto static duration(std::chrono::steady_clock::time_point start_) -> String;

public:
    explicit CVfrb(SPtr<config::CConfiguration> conf_);  ///< @param config The Configuration

    /**
     * @brief The VFRB's main method, runs the VFR-B.
     */
    void
    Run() noexcept;
};
}  // namespace vfrb
