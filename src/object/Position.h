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
    /**
     * @brief GpsPosition
     */
    GpsPosition();

    /**
     * @brief GpsPosition
     * @param vPriority
     * @param vGround
     */
    explicit GpsPosition(std::uint32_t vPriority, bool vGround = false);

    /**
     * @brief GpsPosition
     * @param crPosition
     * @param vGeoid
     * @param vGround
     */
    GpsPosition(const Position& crPosition, double vGeoid, bool vGround);

    /**
     * @brief ~GpsPosition
     */
    virtual ~GpsPosition() noexcept;

    /**
     * @brief assign
     * @param crOther
     */
    void assign(const Object& crOther) override;

    /**
     * Define and declare getters and setters.
     */
    GETSET_CR(Position, mPosition, Position)
    GETSET_V(double, mGeoid, Geoid)
    GETSET_V(std::int32_t, mNrOfSatellites, NrOfSatellites)
    GETSET_V(std::int32_t, mFixQuality, FixQuality)
    GETSET_V(double, mDilution, Dilution)
    GETSET_V(bool, mGround, Ground)

private:
    /// @var mPosition
    /// The position
    Position mPosition{0.0, 0.0, 0};

    /// @var mGeoid
    /// The geoid separation
    double mGeoid = 0.0;

    /// @var mNrOfSatellites
    /// The number of satellites
    std::int32_t mNrOfSatellites = 1;

    /// @var mFixQuality
    /// The GPS fix quality
    std::int32_t mFixQuality = 5;

    /// @var mDilution
    /// The position dilution
    double mDilution = 0.0;

    /// @var mGround
    /// Is the position fixed on ground
    bool mGround = false;
};
}  // namespace object
