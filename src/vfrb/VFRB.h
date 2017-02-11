/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

#ifndef VFRB_H_
#define VFRB_H_

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include "../util/Parameters.h"
class NMEAServer;

class AircraftContainer;
class ClimateData;

#define SYNC_TIME VFRB_SYNC_TIME

class VFRB
{
public:
    VFRB(const VFRB&) = delete;
    VFRB& operator=(const VFRB&) = delete;

    VFRB();
    virtual ~VFRB() throw ();

    /**
     * runs the tool
     */
    static void run();

    /**
     * configuration
     */
    static bool global_climate_enabled;
    static bool global_aprsc_enabled;
    static bool global_sbs_enabled;
    static bool global_run_status;

    static AircraftContainer ac_cont;
    static ClimateData climate_data;

private:
    /**
     * funtions for every single thread
     */
    static void handleSBSInput(boost::asio::signal_set& sigset);
    static void handleAPRSCInput(boost::asio::signal_set& sigset);
    static void handleClimateInput(boost::asio::signal_set& sigset);
    static void handleNMAEServer(NMEAServer& server);

    static void handleSignals(const boost::system::error_code& ec, const int sig);
};

#endif /* VFRB_H_ */
