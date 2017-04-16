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

#ifndef SRC_AIRCRAFT_AIRCRAFTPROCESSOR_H_
#define SRC_AIRCRAFT_AIRCRAFTPROCESSOR_H_

#include <cstdint>
#include <string>

class Aircraft;

#define AP_BUFF_S 8191
#define AP_L_BUFF_S 128

class AircraftProcessor
{
public:
    AircraftProcessor();
    AircraftProcessor(double /*b_lat*/, double /*b_long*/, std::int32_t /*b_alt*/);
    virtual ~AircraftProcessor() noexcept;

    /**
     * build nmea-msg from Aircraft
     */
    std::string process(Aircraft& /*ac*/);

    void init(double /*lat*/, double /*lon*/, std::int32_t /*alt*/);

private:
    /**
     * format string buffer
     */
    char mBuffer[AP_BUFF_S + 1];

    /**
     * calculate relative position to base
     */
    void calcRelPosToBase(Aircraft& /*ac*/);

    /**
     * base position info
     */
    double mBaseLat, mBaseLong,
    /**
     * Longitude base, Aircraft
     * Latitude base, Aircraft
     */
    mRadLongB = 0.0, mtRadLongAc = 0.0, mRadLatB = 0.0, mtRadLatAc = 0.0,
    /**
     * Longitude, Latitude distance
     */
    mtLongDist = 0.0, mtLatDist = 0.0,
    /**
     * relative bearing, absolute bearing
     */
    mtBearingRel = 0.0, mtBearingAbs = 0.0,
    /**
     * value to calculate distance
     */
    mtAval = 0.0;
    /**
     * (alt = height + antennaheight)
     */
    std::int32_t mBaseAlt,
    /**
     * relative North, East, Vertical
     */
    mtRelN = 0, mtRelE = 0, mtRelV = 0,
    /**
     * distance from base position to Aircraft
     */
    mtDist = 0;

};

#endif /* SRC_AIRCRAFT_AIRCRAFTPROCESSOR_H_ */
