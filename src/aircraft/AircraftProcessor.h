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

namespace aircraft
{

class Aircraft;

#define AP_BUFF_S   8191
#define AP_L_BUFF_S 128

/**
 * The AircraftProcessor class.
 *
 * Provide functionality to process Aircraft objects
 * relative to the current base position.
 */
class AircraftProcessor
{
public:
    /**
     * Constructor
     */
    AircraftProcessor();
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~AircraftProcessor() noexcept;
    /**
     * Process an Aircraft relative to the base position.
     * Result as PFLAU + PFLAA NMEA sentence, with <cr><lf> postfix.
     *
     * @param cr_ac the given Aircraft
     *
     * @return the NMEA string
     */
    std::string process(const Aircraft& cr_ac);

private:
    /**
     * Calcutale Aircrafts relative position to the base.
     *
     * @param cr_ac the given Aircraft
     */
    void calcRelPosToBase(const Aircraft& cr_ac);

    /// Formatstring buffer
    char mBuffer[AP_BUFF_S + 1];
    /// Temporary values
    /// Bases longitude as radian
    double mtRadLongB = 0.0,
    /// Aircrafts longitude as radian
            mtRadLongAc = 0.0,
            /// Bases latitude as radian
            mtRadLatB = 0.0,
            /// Aircrafts latitude as radian
            mtRadLatAc = 0.0,
            /// Distance/Difference between Aircrafts and bases longitude
            mtLongDist = 0.0,
            /// Distance/Difference between Aircrafts and bases latitude
            mtLatDist = 0.0,
            /// Relative bearing
            mtBearingRel = 0.0,
            /// Absolute bearing
            mtBearingAbs = 0.0;
    /// Relative distance in northern direction; m
    std::int32_t mtRelN = 0,
    /// Relative distance in eastern direction; m
            mtRelE = 0,
            /// Relative vertical distance; m
            mtRelV = 0,
            /// Distance between Aircraft and base; m
            mtDist = 0;

};

}  // namespace aircraft

#endif /* SRC_AIRCRAFT_AIRCRAFTPROCESSOR_H_ */
