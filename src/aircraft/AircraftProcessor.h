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

#ifndef AIRCRAFTPROCESSOR_H_
#define AIRCRAFTPROCESSOR_H_

#include <cstdint>
#include <string>

class Aircraft;

#define AP_BUFF_S 8191
#define AP_L_BUFF_S 128

class AircraftProcessor
{
public:
    AircraftProcessor();
    AircraftProcessor(double, double, std::int32_t);
    virtual ~AircraftProcessor();

    /**
     * build nmea-msg from Aircraft
     */
    std::string process(Aircraft&);

    void init(double, double, std::int32_t);

private:
    /**
     * format string buffer
     */
    char buffer[AP_BUFF_S + 1];

    /**
     * calculate relative position to base
     */
    void calcRelPosToBase(Aircraft&);

    /**
     * base position info
     */
    double baselat, baselong,
    /**
     * Longitude base, Aircraft
     * Latitude base, Aircraft
     */
    long_b = 0.0, long_ac = 0.0, lat_b = 0.0, lat_ac = 0.0,
    /**
     * Longitude, Latitude distance
     */
    long_dist = 0.0, lat_dist = 0.0,
    /**
     * relative bearing, absolute bearing
     */
    bearing_rel = 0.0, bearing_abs = 0.0,
    /**
     * values to calculate distance
     */
    a = 0.0;
    /**
     * (alt = height + antennaheight)
     */
    std::int32_t basealt,
    /**
     * relative North, East, Vertical
     */
    rel_N = 0, rel_E = 0, rel_V = 0,
    /**
     * distance from base position to Aircraft
     */
    dist = 0;

};

#endif /* AIRCRAFTPROCESSOR_H_ */
