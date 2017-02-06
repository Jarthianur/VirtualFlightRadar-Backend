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

#define A_UKNWN_T 0
#define A_ADSB_T 8
#define A_MIN_DATA -1
#define A_VALUE_NA -1000.0

class Aircraft
{
public:
    Aircraft(std::string& id, double lat, double lon, int32_t alt);
    Aircraft(std::string& id, double lat, double lon, int32_t alt, double gnd_spd,
            uint32_t id_t, int32_t ac_t, double climb_r, double turn_r, double heading);
    virtual ~Aircraft() throw ();

    /**
     * self explanatory
     */
    std::string id;
    // m/s
    double gnd_speed = 0;
    uint32_t id_type = 1;
    int32_t aircraft_type = A_MIN_DATA;
    /**
     * absolue position
     */
    double latitude = 0.0;
    double longitude = 0.0;
    // m
    int32_t altitude = 0;
    // deg [0-359]
    double heading = 0;
    /**
     * relative to last position
     */
    // m/s
    double climb_rate = 0.0;
    // deg/s
    double turn_rate = 0.0;
    //0 = valid; +x(cycles) = invalid
    uint32_t valid = 0;
    //differ altitude (GPS from aprsc, QNE from sbs)
    bool qne = false;

};

#endif /* AIRCRAFT_H_ */
