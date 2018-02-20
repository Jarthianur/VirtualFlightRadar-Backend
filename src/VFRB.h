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

#ifndef SRC_VFRB_H_
#define SRC_VFRB_H_

#include <atomic>
#include <memory>
#include <list>
#include <boost/asio/signal_set.hpp>
#include <boost/system/error_code.hpp>
#include "network/server/Server.h"


namespace config {
class Configuration;
}
namespace data
{
class AircraftData;
class WindData;
class AtmosphereData;
class GpsData;
class Data;
}

namespace feed
{
class Feed;
}
namespace aircraft
{
class Aircraft;
}
namespace util
{
struct ExtGpsPosition;
struct SensorInfo;
}

/**
 * @class VFRB
 * @brief Combine all features and is the main entry point for the actual VFR-B.
 */
class VFRB
{
public:
    /**
     * Non-copyable
     */
    VFRB(const VFRB&) = delete;
    /**
     * Not assignable
     */
    VFRB& operator=(const VFRB&) = delete;
    /**
     * @fn VFRB
     * @brief Constructor
     */
    explicit VFRB(const config::Configuration& crConfig);
    /**
     * @fn ~VFRB
     * @brief Destructor
     */
    virtual ~VFRB() noexcept;
    /**
     * @fn run
     * @brief The VFRB's main method, runs the VFR-B.
     */
    void run() noexcept;

    /// Atomic run-status. By this, every component may determine if the VFRB stops.
    static std::atomic<bool> global_run_status;

private:
       /**
     * @fn registerFeeds
     * @brief Register all input feeds found from ConfigReader.
     * @note Only correctly configured feeds get registered.
     * @param crProperties The PropertyMap holding read properties
     * @return the number of registered feeds
     */
    void registerFeeds(const config::Configuration& crFeeds);

       /// Container holding all registered Aircrafts
    std::shared_ptr<data::AircraftData> mpAircraftData;

    /// Container holding sensor and climate information.
    std::shared_ptr<data::WindData> mpWindData;

    ///
    std::shared_ptr<data::AtmosphereData> mpAtmosphereData;

    /// Container holding GPS information
    std::shared_ptr<data::GpsData> mpGpsData;

    network::server::Server mServer;

    std::list<std::shared_ptr<feed::Feed> > mFeeds;
};

#endif /* SRC_VFRB_H_ */
