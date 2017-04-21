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

#include <boost/asio/signal_set.hpp>
#include <boost/system/error_code.hpp>
#include <atomic>

class Feed;
class AircraftContainer;
class SensorData;
class NMEAServer;
class GPSData;

class VFRB
{
public:
    VFRB(const VFRB&) = delete;
    VFRB& operator=(const VFRB&) = delete;

    VFRB();
    virtual ~VFRB() noexcept;

    /**
     * runs the tool
     */
    static void run() noexcept;

    /**
     * configuration
     */
    static std::atomic<bool> global_run_status;

    /**
     * Container for aircrafts
     */
    static AircraftContainer msAcCont;
    /**
     * Container for climate information
     */
    static SensorData msSensorData;
    /**
     * Container for GPS information
     */
    static GPSData msGPSdata;

private:
    /**
     * handlers for threads
     */
    static void handleInputFeed(boost::asio::signal_set& /*sigset*/, Feed& /*feed*/);
    static void handleNMAEServer(NMEAServer& server);
    static void handleSignals(const boost::system::error_code& /*ec*/, const int /*sig*/);
};

#endif /* SRC_VFRB_VFRB_H_ */
