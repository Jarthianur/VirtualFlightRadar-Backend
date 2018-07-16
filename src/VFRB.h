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
#include <list>
#include <memory>
#include <boost/chrono.hpp>

#include "feed/client/ClientManager.h"
#include "server/Server.h"
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
    NON_COPYABLE(VFRB)

    /**
     * @fn VFRB
     * @brief Constructor
     * @param crConfig The configuration
     */
    explicit VFRB(const config::Configuration& crConfig);

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

    /// @var vRunStatus
    /// Atomic run-status. By this, every component may determine if the VFRB stops.
    static std::atomic<bool> vRunStatus;

private:
    /**
     * @fn createFeeds
     * @brief Register all input Feeds.
     * @param crConfig The Configuration
     */
    void createFeeds(const config::Configuration& crConfig);

    /**
     * @fn setupSignals
     * @brief Setup the signal set.
     * @param rSigSet
     */
    void setupSignals(boost::asio::signal_set& rSigSet);

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
    std::string getDuration(boost::chrono::steady_clock::time_point vStart) const;

    /// @var mpAircraftData
    /// Manage aircrafts
    std::shared_ptr<data::AircraftData> mpAircraftData;

    /// @var mpAtmosphereData
    /// Manage atmospheric data
    std::shared_ptr<data::AtmosphereData> mpAtmosphereData;

    /// @var mpGpsData
    /// Manage GPS data
    std::shared_ptr<data::GpsData> mpGpsData;

    /// @var mpWindData
    /// Manage wind data
    std::shared_ptr<data::WindData> mpWindData;

    /// @var mServer
    /// Manage clients and sending of data
    server::Server mServer;

    feed::client::ClientManager mClientManager;

    /// @var mFeeds
    /// List of all active feeds
    std::list<std::shared_ptr<feed::Feed>> mFeeds;
};
