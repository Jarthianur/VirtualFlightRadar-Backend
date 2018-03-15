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

#include "Object.h"

/// @namespace data
namespace data
{
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
    explicit GpsPosition(std::uint32_t vPriority, bool vGround = false);
    GpsPosition(const Position& crPosition, double vGeoid, bool vGround);
    virtual ~GpsPosition() noexcept;

    void assign(const Object& crOther) override;

    /// Position
    Position position{0.0, 0.0, 0};
    /// Number of satellites
    std::int32_t nrSats = 1;
    /// GPS fix quality
    std::int32_t fixQa = 5;
    /// Geoid separation
    double geoid    = 0.0;
    double dilution = 0.0;
    bool ground     = false;
};
}  // namespace object
}  // namespace data
