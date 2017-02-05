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

#include "../connection/NMEAServer.h"

class AircraftContainer;
class ClimateData;

#define SYNC_TIME 1
#define WAIT_TIME 30

class VFRB
{
public:
    VFRB();
    virtual ~VFRB() throw ();

    /**
     * runs the tool
     */
    static void run();

    /**
     * configuration
     */
    static bool global_weather_feed_enabled;
    static bool global_aprs_enabled;
    static bool global_sbs_enabled;
    static bool global_run_status;

    static AircraftContainer ac_cont;
    static ClimateData climate_data;
protected:
    /**
     * funtions for every single thread
     */
    static void handle_sbs_in(AircraftContainer&);
    static void handle_aprs_in(AircraftContainer&);
    static void handle_con_out(NMEAServer&);
    static void handle_weather_feed(ClimateData&);
    static void exit_signal_handler(int sig);
};

#endif /* VFRB_H_ */
