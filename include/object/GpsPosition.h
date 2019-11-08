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
struct SLocation
{
    inline static constexpr auto MAX_LATITUDE  = 90.0;
    inline static constexpr auto MIN_LATITUDE  = -90.0;
    inline static constexpr auto MAX_LONGITUDE = 180.0;
    inline static constexpr auto MIN_LONGITUDE = 0.0;
    inline static constexpr auto MAX_ALTITUDE  = 100000;
    inline static constexpr auto MIN_ALTITUDE  = -11000;

    f64 Latitude;   ///< Latitude; deg
    f64 Longitude;  ///< Longitude; deg
    s32 Altitude;   ///< Altitude; m
};

/**
 * @brief Extend Object to a GPS position.
 */
class CGpsPosition : public CObject
{
public:
    inline static constexpr auto MAX_GEOID = 86.0;
    inline static constexpr auto MIN_GEOID = -108.0;

private:
    SLocation  m_location;        ///< The location
    f64        m_geoid;           ///< The geoid separation
    f64        m_dilution;        ///< The position dilution
    u8         m_nrOfSatellites;  ///< The number of satellites
    s8         m_fixQuality;      ///< The GPS fix quality
    CTimestamp m_timestamp;       ///< The timestamp of this position

    /**
     * @brief Override Object::assign.
     */
    void assign(CObject&& other_) override;

    /**
     * @brief Override Object::canUpdate.
     */
    bool canUpdate(CObject const& other_) const override;

public:
    CGpsPosition(u32 priority_, SLocation const& location_, f64 geoid_);

    /**
     * @param position The position
     * @param geoid    The geoid
     */
    CGpsPosition(u32 priority_, SLocation const& location_, f64 geoid_, f64 dilution_, u8 satellites_,
                 s8 quality_, CTimestamp const& timestamp_);
    ~CGpsPosition() noexcept override = default;

    auto Location() const -> decltype(m_location) const&;
    auto Geoid() const -> decltype(m_geoid);
    auto Timestamp() const -> decltype(m_timestamp) const&;
    auto Dilution() const -> decltype(m_dilution);
    auto NrOfSatellites() const -> decltype(m_nrOfSatellites);
    auto FixQuality() const -> decltype(m_fixQuality);
};
}  // namespace vfrb::object
