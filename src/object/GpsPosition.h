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

#pragma once

#include <cstdint>

#include "../Defines.h"
#include "Object.h"
#include "TimeStamp.h"

/// @namespace object
namespace object
{
/**
 * @struct Position
 * @brief Represent a position on earth.
 */
struct Position
{
    /// @var latitude
    /// Latitude; deg
    double latitude;

    /// @var longitude
    /// Longitude; deg
    double longitude;

    /// @var altitude
    /// Altitude; m
    std::int32_t altitude;
};

/**
 * @class GpsPosition
 * @brief Represent a position and provide GPS information.
 * @extends Object
 */
class GpsPosition : public Object
{
public:
    GpsPosition();

    /**
     * @fn GpsPosition
     * @brief Constructor
     * @param vPriority The initial priority
     */
    explicit GpsPosition(std::uint32_t priority);

    /**
     * @fn GpsPosition
     * @brief Constructor
     * @param crPosition The position
     * @param vGeoid     The geoid
     */
    GpsPosition(const Position& position, double geoid);

    ~GpsPosition() noexcept;

private:
    /**
     * @see Object#assign
     */
    void assign(Object&& other) override;

    /**
     * @fn canUpdate
     * @brief Check whether this Aircraft can update the other one.
     * @param crOther   The other Aircraft
     * @param vAttempts The update attempt count
     * @return true if yes, else false
     */
    bool canUpdate(const Object& other) const override;

    /// @var mPosition
    /// The position
    Position m_position{0.0, 0.0, 0};

    /// @var mGeoid
    /// The geoid separation
    double m_geoid = 0.0;

    /// @var mTimeStamp
    /// The timestamp of this position
    TimeStamp m_timeStamp;

    /// @var mNrOfSatellites
    /// The number of satellites
    std::int32_t m_nrOfSatellites = 1;

    /// @var mFixQuality
    /// The GPS fix quality
    std::int32_t m_fixQuality = 5;

    /// @var mDilution
    /// The position dilution
    double m_dilution = 0.0;

public:
    /**
     * Define and declare getters and setters.
     */
    GETSET_CR(position)
    GETSET_V(geoid)
    GETSET_V(timeStamp)
    GETSET_V(nrOfSatellites)
    GETSET_V(fixQuality)
    GETSET_V(dilution)
};
}  // namespace object
