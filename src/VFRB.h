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
#include <boost/asio/signal_set.hpp>
#include <boost/system/error_code.hpp>
#include <memory>

namespace data
{
class AircraftContainer;
class SensorData;
class GpsData;
}
namespace tcp
{
namespace server
{
class Server;
}
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
struct ExtGPSPosition;
struct SensorInfo;
}

/**
 * The VFRB class.
 *
 * This class combines all the functionality of VFR-B.
 * Expects a valid Configuration.
 * The whole VFR-B may be started by a single call of VFRB::run.
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
     * Constructor
     */
    VFRB();
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~VFRB() noexcept;
    /**
     * The VFRB's main method.
     * This function spawns all threads and
     * actually runs the VFR-B.
     *
     * @exceptsafe no-throw
     */
    static void run() noexcept;

    static void pushAircraft(const aircraft::Aircraft& cr_ac, std::int32_t prio);
    static void pushSensorInfo(const util::SensorInfo& cr_si, std::int32_t prio);
    static void pushGpsPosition(const util::ExtGPSPosition& cr_pos, std::int32_t prio);

    /// Atomic run-status. By this, every component may determine if the VFRB stops.
    static std::atomic<bool> global_run_status;

private:
    /**
     * Handler for an input Feed thread.
     *
     * @param r_sigset the signal set to pass
     * @param p_feed   the Feed to handle
     */
    static void handleFeed(boost::asio::signal_set& r_sigset,
            std::shared_ptr<feed::Feed> p_feed);
    /**
     * Handler for an Server thread.
     *
     * @param r_server the Server to handle
     */
    static void handleServer(tcp::server::Server& r_server);
    /**
     * Handler for a signal interrupt thread.
     *
     * @param cr_ec the error code
     * @param sig   the signal number
     */
    static void handleSignals(const boost::system::error_code& cr_ec, const int sig);
    /// Container holding all registered Aircrafts
    static data::AircraftContainer msAcCont;
    /// Container holding sensor and climate information.
    static data::SensorData msSensorData;
    /// Container holding GPS information
    static data::GpsData msGpsData;
};

#endif /* SRC_VFRB_H_ */
