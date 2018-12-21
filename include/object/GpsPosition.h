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

#include "util/defines.h"

#include "object/DateTimeImplBoost.h"
#include "object/Object.h"
#include "object/TimeStamp.hpp"

namespace object
{
/**
 * @brief A position on earth.
 */
struct Position
{
    /// Latitude; deg
    double latitude;

    /// Longitude; deg
    double longitude;

    /// Altitude; m
    std::int32_t altitude;
};

/**
 * @brief Extend Object to a GPS position.
 */
class GpsPosition : public Object
{
public:
    DEFAULT_DTOR(GpsPosition)

    GpsPosition();

    /**
     * @brief Constructor
     * @param priority The initial priority
     */
    explicit GpsPosition(std::uint32_t priority);

    /**
     * @brief Constructor
     * @param position The position
     * @param geoid    The geoid
     */
    GpsPosition(const Position& position, double geoid);

private:
    /**
     * @brief Override Object::assign.
     */
    void assign(Object&& other) override;

    /**
     * @brief Override Object::canUpdate.
     */
    bool canUpdate(const Object& other) const override;

    /// The position
    Position m_position{0.0, 0.0, 0};

    /// The geoid separation
    double m_geoid = 0.0;

    /// The timestamp of this position
    TimeStamp<timestamp::DateTimeImplBoost> m_timeStamp;

    /// The position dilution
    double m_dilution = 0.0;

    /// The number of satellites
    std::uint8_t m_nrOfSatellites = 1;

    /// The GPS fix quality
    std::int8_t m_fixQuality = 5;

public:
    /**
     *Getters and setters
     */
    GETSET_CR(position)
    GETSET_V(geoid)
    GETSET_V(timeStamp)
    GETSET_V(nrOfSatellites)
    GETSET_V(fixQuality)
    GETSET_V(dilution)
};
}  // namespace object
