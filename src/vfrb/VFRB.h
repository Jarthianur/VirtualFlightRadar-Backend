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

#ifndef SRC_VFRB_VFRB_H_
#define SRC_VFRB_VFRB_H_

#include <atomic>
#include <boost/asio/signal_set.hpp>
#include <boost/system/error_code.hpp>

namespace data
{
class AircraftContainer;
class SensorData;
class GPSData;
}
namespace tcp
{
namespace server
{
class Server;
}
}

namespace vfrb
{

class Feed;

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

    /// Atomic run-status. By this, every component may determine if the VFRB stops.
    static std::atomic<bool> global_run_status;
    /// Container holding all registered Aircrafts
    static data::AircraftContainer msAcCont;
    /// Container holding sensor and climate information.
    static data::SensorData msSensorData;
    /// Container holding GPS information
    static data::GPSData msGPSdata;

private:
    /**
     * Handler for an input Feed thread.
     *
     * @param r_sigset the signal set to pass
     * @param r_feed   the Feed to handle
     */
    static void handleInputFeed(boost::asio::signal_set& /*r_sigset*/,
                                Feed& /*r_feed*/);
    /**
     * Handler for an Server thread.
     *
     * @param r_server the Server to handle
     */
    static void handleNMAEServer(tcp::server::Server& /*r_server*/);
    /**
     * Handler for a signal interrupt thread.
     *
     * @param cr_ec the error code
     * @param sig   the signal number
     */
    static void handleSignals(const boost::system::error_code& /*cr_ec*/,
                              const int /*sig*/);
};

}  // namespace vfrb

#endif /* SRC_VFRB_VFRB_H_ */
