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

#ifndef AIRCRAFT_H_
#define AIRCRAFT_H_

#include <cstdint>
#include <string>

#define A_ADSB_T 8
#define A_VALUE_NA -1000.0

class Aircraft
{
public:
    Aircraft(std::string& id, double lat, double lon, int32_t alt);
    Aircraft(std::string& id, double lat, double lon, int32_t alt, double gnd_spd,
            uint32_t id_t, int32_t ac_t, double climb_r, double turn_r, double heading);
    virtual ~Aircraft() throw ();

    // id
    std::string id;
    // m/s
    double gnd_speed = A_VALUE_NA;
    uint32_t id_type = 1;
    int32_t aircraft_type = A_ADSB_T;
    /**
     * absolute position
     */
    double latitude = A_VALUE_NA;
    double longitude = A_VALUE_NA;
    // m
    int32_t altitude = 0;
    // deg [0-359]
    double heading = A_VALUE_NA;
    /**
     * relative to last position
     */
    // m/s
    double climb_rate = A_VALUE_NA;
    // deg/s
    double turn_rate = A_VALUE_NA;
    /**
     * info
     */
    // full info available
    bool full_info = false;
    //0 = valid; +x(cycles) = invalid
    uint32_t valid = 0;
    //differ altitude (GPS from aprs, QNE from sbs)
    bool qne = false;
    // as long as FLARM input (GPS height) is available prefer
    bool flarm_target = true;
};

#endif /* AIRCRAFT_H_ */
