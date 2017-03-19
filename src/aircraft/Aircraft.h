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

#ifndef SRC_AIRCRAFT_AIRCRAFT_H_
#define SRC_AIRCRAFT_AIRCRAFT_H_

#include <cstdint>
#include <string>

#define A_ADSB_T 8
#define A_VALUE_NA -1024.0

class Aircraft
{
public:
    Aircraft(std::string& /*id*/, double /*lat*/, double /*lon*/, std::int32_t /*alt*/);
    Aircraft(std::string& /*id*/, double /*lat*/, double /*lon*/, std::int32_t /*alt*/,
             double /*gnd_spd*/, std::uint32_t /*id_t*/, std::int32_t /*ac_t*/,
             double /*climb_r*/, double /*turn_r*/, double /*heading*/);
    virtual ~Aircraft() noexcept;

    bool operator==(const Aircraft&) const;

    enum class TargetType
        : std::uint32_t
        {
            FLARM,
        TRANSPONDER
    };

    const std::string& getID() const;

    /**
     * information
     */
    std::uint32_t mIDtype = 1;
    std::int32_t mAircraftType = A_ADSB_T;
    TargetType mTargetType = TargetType::FLARM;

    // full info available
    bool mFullInfo = false;

    //0 = valid; +x(cycles) = invalid
    std::uint32_t mValid = 0;

    //differ altitude (GPS from FLARM, QNE from TRANSPONDER)
    bool mAltAsQNE = false;

    /**
     * position
     */
    // deg
    double mLatitude = A_VALUE_NA;
    double mLongitude = A_VALUE_NA;
    // m
    std::int32_t mAltitude = 0;

    /**
     * movement
     */
    // m/s
    double mGndSpeed = A_VALUE_NA;

    // deg [0-359]
    double mHeading = A_VALUE_NA;

    // m/s
    double mClimbRate = A_VALUE_NA;

    // currently unused
    // deg/s
    double mTurnRate = A_VALUE_NA;

private:
    // id
    std::string mID;
};

#endif /* SRC_AIRCRAFT_AIRCRAFT_H_ */
