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

namespace data
{
namespace object
{
/**
 * @class GpsPosition
 * @brief Minimal information for a position.
 */
struct GpsPosition
{
    /// Latitude; deg
    double latitude;
    /// Longitude; deg
    double longitude;
    /// Altitude; m
    std::int32_t altitude;
};

/**
 * @class ExtGpsPosition
 * @brief GPS position and meta information.
 */
class ExtGpsPosition : public Object
{
public:
    ExtGpsPosition();
    explicit ExtGpsPosition(std::uint32_t vPriority);
    virtual ~ExtGpsPosition() noexcept;
    Object& operator=(const boost::tuple<const Object&, std::uint32_t>& crOther) override;

    /// Position
    struct GpsPosition position;
    /// Number of satellites
    std::int32_t nrSats;
    /// GPS fix quality
    std::int32_t fixQa;
    /// Geoid separation
    double geoid;
    double dilution;
};
}  // namespace object
}  // namespace data
