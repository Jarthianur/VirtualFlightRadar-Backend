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
#include <string>

#include "server/Server.hpp"
#include "server/SocketImplBoost.h"
#include "server/TcpInterfaceImplBoost.h"
#include "Defines.h"

namespace config
{
class Configuration;
} /* namespace config */
namespace data
{
class AircraftData;
class AtmosphereData;
class GpsData;
class WindData;
} /* namespace data */
namespace feed
{
class Feed;
} /* namespace feed */

/**
 * @class VFRB
 * @brief Combine all features and is the main entry point for the actual VFR-B.
 */
class VFRB
{
public:
    NOT_COPYABLE(VFRB)

    /**
     * @fn VFRB
     * @brief Constructor
     * @param crConfig The configuration
     */
    explicit VFRB(const config::Configuration& config);

    /**
     * @fn ~VFRB
     * @brief Destructor
     */
    ~VFRB() noexcept;

    /**
     * @fn run
     * @brief The VFRB's main method, runs the VFR-B.
     */
    void run() noexcept;

private:
    /**
     * @fn createFeeds
     * @brief Register all input Feeds.
     * @param crConfig The Configuration
     */
    void createFeeds(const config::Configuration& config);

    /**
     * @fn serve
     * @brief Serve the data frequently every second using the Server.
     */
    void serve();

    /**
     * @fn getDuration
     * @brief Get the duration from given start value as formatted string.
     * @param vStart The start value
     * @return the duration string
     */
    std::string get_duration(std::chrono::steady_clock::time_point start) const;

    /// @var mpAircraftData
    /// Manage aircrafts
    std::shared_ptr<data::AircraftData> m_aircraftData;

    /// @var mpAtmosphereData
    /// Manage atmospheric data
    std::shared_ptr<data::AtmosphereData> m_atmosphereData;

    /// @var mpGpsData
    /// Manage GPS data
    std::shared_ptr<data::GpsData> m_gpsData;

    /// @var mpWindData
    /// Manage wind data
    std::shared_ptr<data::WindData> m_windData;

    /// @var mServer
    /// Manage clients and sending of data
    server::Server<server::TcpInterfaceImplBoost, server::SocketImplBoost> m_server;

    /// @var mFeeds
    /// List of all active feeds
    std::list<std::shared_ptr<feed::Feed>> m_feeds;

    /// @var vRunStatus
    /// Atomic run-status.
    std::atomic<bool> m_running;
};
