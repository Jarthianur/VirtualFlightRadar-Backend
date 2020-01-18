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

#pragma once

#include <atomic>
#include <chrono>
#include <list>

#include "server/Server.hpp"
#include "server/net/impl/AcceptorBoost.h"
#include "server/net/impl/SocketBoost.h"
#include "util/class_utils.h"

#include "types.h"

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
    NOT_COPYABLE(CVfrb)

    SPtr<data::CAircraftData>                  m_aircraftData;    ///< Aircraft container
    SPtr<data::CAtmosphereData>                m_atmosphereData;  ///< Atmospheric data container
    SPtr<data::CGpsData>                       m_gpsData;         ///< GPS data container
    SPtr<data::CWindData>                      m_windData;        ///< Wind data container
    server::CServer<server::net::CSocketBoost> m_server;          ///< Manage clients and sending of data
    std::list<SPtr<feed::IFeed>>               m_feeds;           ///< List of all active feeds
    std::atomic<bool>                          m_running;         ///< Atomic run-status

    /**
     * @brief Create all input feeds.
     * @param config The Configuration
     */
    void createFeeds(SPtr<config::CConfiguration> conf_);

    /**
     * @brief Serve the data frequently every second.
     */
    void serve();

    /**
     * @brief Get the duration from given start value as formatted string.
     * @param start The start value
     * @return the duration string
     */
    Str duration(std::chrono::steady_clock::time_point start_) const;

public:
    explicit CVfrb(SPtr<config::CConfiguration> conf_);  ///< @param config The Configuration
    ~CVfrb() noexcept = default;

    /**
     * @brief The VFRB's main method, runs the VFR-B.
     */
    void Run() noexcept;
};
}  // namespace vfrb
