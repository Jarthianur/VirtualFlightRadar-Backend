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

#include "Object.h"
#include "Timestamp.h"
#include "types.h"

namespace vfrb::object
{
/**
 * @brief A position on earth.
 */
struct Location
{
    inline static constexpr auto MAX_LATITUDE  = 90.0;
    inline static constexpr auto MIN_LATITUDE  = -90.0;
    inline static constexpr auto MAX_LONGITUDE = 180.0;
    inline static constexpr auto MIN_LONGITUDE = 0.0;
    inline static constexpr auto MAX_ALTITUDE  = 100000;
    inline static constexpr auto MIN_ALTITUDE  = -11000;

    f64 latitude;   ///< Latitude; deg
    f64 longitude;  ///< Longitude; deg
    s32 altitude;   ///< Altitude; m
};

/**
 * @brief Extend Object to a GPS position.
 */
class GpsPosition : public Object
{
public:
    inline static constexpr auto MAX_GEOID = 86.0;
    inline static constexpr auto MIN_GEOID = -108.0;

private:
    Location  m_location;        ///< The location
    f64       m_geoid;           ///< The geoid separation
    f64       m_dilution;        ///< The position dilution
    u8        m_nrOfSatellites;  ///< The number of satellites
    s8        m_fixQuality;      ///< The GPS fix quality
    Timestamp m_timestamp;       ///< The timestamp of this position

    /**
     * @brief Override Object::assign.
     */
    void assign(Object&& other) override;

    /**
     * @brief Override Object::canUpdate.
     */
    bool canUpdate(Object const& other) const override;

public:
    GpsPosition(u32 priority, Location const& location, f64 geoid);

    /**
     * @param position The position
     * @param geoid    The geoid
     */
    GpsPosition(u32 priority, Location const& location, f64 geoid, f64 dilution, u8 satellites, s8 quality,
                Timestamp const& timestamp);
    ~GpsPosition() noexcept override = default;

    auto location() const -> decltype(m_location) const&;
    auto geoid() const -> decltype(m_geoid);
    auto timestamp() const -> decltype(m_timestamp) const&;
    auto dilution() const -> decltype(m_dilution);
    auto nrOfSatellites() const -> decltype(m_nrOfSatellites);
    auto fixQuality() const -> decltype(m_fixQuality);
};
}  // namespace vfrb::object
