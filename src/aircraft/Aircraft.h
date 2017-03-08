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
#define A_VALUE_NA -1024.0

class Aircraft
{
public:
    Aircraft(std::string& id, double lat, double lon, std::int32_t alt);
    Aircraft(std::string& id, double lat, double lon, std::int32_t alt, double gnd_spd,
             std::uint32_t id_t, std::int32_t ac_t, double climb_r, double turn_r, double heading);
    virtual ~Aircraft() throw ();

    bool operator==(const Aircraft&) const;

    enum class TargetType
        : std::uint32_t
        {
            FLARM, TRANSPONDER
    };

    /**
     * information
     */
    // id
    std::string id;

    std::uint32_t id_type = 1;
    std::int32_t aircraft_type = A_ADSB_T;
    TargetType target_type = TargetType::FLARM;

    // full info available
    bool full_info;

    //0 = valid; +x(cycles) = invalid
    std::uint32_t valid = 0;

    //differ altitude (GPS from FLARM, QNE from TRANSPONDER)
    bool qne_altitude = false;

    /**
     * position
     */
    // deg
    double latitude = A_VALUE_NA;
    double longitude = A_VALUE_NA;
    // m
    std::int32_t altitude = 0;

    /**
     * movement
     */
    // m/s
    double gnd_speed = A_VALUE_NA;

    // deg [0-359]
    double heading = A_VALUE_NA;

    // m/s
    double climb_rate = A_VALUE_NA;

    // currently unused
    // deg/s
    double turn_rate = A_VALUE_NA;
};

#endif /* AIRCRAFT_H_ */
