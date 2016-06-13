/*
Copyright_License {

  Copyright (C) 2016 Virtual Flight Radar - Backend
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

#include "ConnectOutNMEA.h"
#include "AircraftContainer.h"
#include "ConnectInExt.h"
#include "NMEAFeedW.h"

#define SYNC_TIME 1
#define WAIT_TIME 3

class VFRB {
public:
    VFRB();
    virtual ~VFRB() throw();

    /**
     * runs the tool
     */
    static void run();
    /**
     * configuration
     */
    static int base_altitude;
    static long double base_latitude;
    static long double base_longitude;
    static float base_geoid;
    static int global_out_port;
    static int global_ogn_port;
    static int global_adsb_port;
    static std::string global_ogn_host;
    static std::string global_adsb_host;
    static std::string global_login_str;
    static std::string global_nmea_feed_host;
    static int global_nmea_feed_port;
    static int filter_maxHeight;
    static int filter_maxDist;
    static bool global_nmea_feed_enabled;
    static bool global_ogn_enabled;
    static bool global_adsb_enabled;

protected:
    /**
     * funtions for every single thread
     */
    static void handle_adsb_in(AircraftContainer&);
    static void handle_ogn_in(AircraftContainer&);
    static void handle_con_out(ConnectOutNMEA&);
    static void handle_nmea_feed(NMEAFeedW&);
};

#endif /* VFRB_H_ */
