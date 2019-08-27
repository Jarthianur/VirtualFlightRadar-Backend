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
#include <memory>

#include "server/Server.hpp"
#include "server/net/impl/NetworkInterfaceImplBoost.h"
#include "server/net/impl/SocketImplBoost.h"
#include "util/defines.h"
#include "util/types.h"

namespace vfrb::config
{
class Configuration;
}  // namespace vfrb::config
namespace vfrb::data
{
class AircraftData;
class AtmosphereData;
class GpsData;
class WindData;
}  // namespace vfrb::data
namespace vfrb::feed
{
class Feed;
}  // namespace vfrb::feed

namespace vfrb
{
/**
 * @brief Combine all features and is the main entry point for the actual VFR-B.
 */
class VFRB
{
    NOT_COPYABLE(VFRB)

    s_ptr<data::AircraftData>                    m_aircraftData;    ///< Aircraft container
    s_ptr<data::AtmosphereData>                  m_atmosphereData;  ///< Atmospheric data container
    s_ptr<data::GpsData>                         m_gpsData;         ///< GPS data container
    s_ptr<data::WindData>                        m_windData;        ///< Wind data container
    server::Server<server::net::SocketImplBoost> m_server;          ///< Manage clients and sending of data
    std::list<s_ptr<feed::Feed>>                 m_feeds;           ///< List of all active feeds
    std::atomic<bool>                            m_running;         ///< Atomic run-status

    /**
     * @brief Create all input feeds.
     * @param config The Configuration
     */
    void createFeeds(s_ptr<config::Configuration> config);

    /**
     * @brief Serve the data frequently every second.
     */
    void serve();

    /**
     * @brief Get the duration from given start value as formatted string.
     * @param start The start value
     * @return the duration string
     */
    str duration(std::chrono::steady_clock::time_point start) const;

public:
    explicit VFRB(s_ptr<config::Configuration> config);  ///< @param config The Configuration
    ~VFRB() noexcept = default;

    /**
     * @brief The VFRB's main method, runs the VFR-B.
     */
    void run() noexcept;
};
}  // namespace vfrb
