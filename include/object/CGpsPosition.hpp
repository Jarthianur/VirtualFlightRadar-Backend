/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "util/ClassUtils.hpp"

#include "CObject.hpp"
#include "CTimestamp.hpp"
#include "Types.hpp"

namespace vfrb::object
{
/// A 3D position on earth
struct SLocation
{
    CTCONST MAX_LATITUDE  = 90.0;
    CTCONST MIN_LATITUDE  = -90.0;
    CTCONST MAX_LONGITUDE = 180.0;
    CTCONST MIN_LONGITUDE = -180.0;
    CTCONST MAX_ALTITUDE  = 100000;
    CTCONST MIN_ALTITUDE  = -11000;

    f64 Latitude;   ///< Latitude; deg
    f64 Longitude;  ///< Longitude; deg
    i32 Altitude;   ///< Altitude; m
};

/// GPS position object
class CGpsPosition : public CObject
{
    SLocation  m_location;        ///< The location
    f64        m_geoid;           ///< The geoid separation
    f64        m_dilution;        ///< The position dilution
    u8         m_nrOfSatellites;  ///< The number of satellites
    i8         m_fixQuality;      ///< The GPS fix quality
    CTimestamp m_timestamp;       ///< The timestamp of this position

    void
    assign(CObject&& other_) override;
    [[nodiscard]] auto
    canUpdate(CObject const& other_) const -> bool override;

public:
    DEFAULT_COPYABLE(CGpsPosition)
    COPY_ON_MOVE(CGpsPosition)

    CTCONST MAX_GEOID = 86.0;
    CTCONST MIN_GEOID = -108.0;

    /// @param prio_ The initial priority
    CGpsPosition(u32 prio_, SLocation const& loc_, f64 geo_);

    /// @param prio_ The initial priority
    CGpsPosition(u32 prio_, SLocation const& loc_, f64 geo_, f64 dil_, u8 sat_, i8 qual_,
                 CTimestamp const& ts_);
    ~CGpsPosition() noexcept override = default;

    GETTER_REF(Location, m_location)
    GETTER_REF(Timestamp, m_timestamp)
    GETTER(Geoid, m_geoid)
    GETTER(Dilution, m_dilution)
    GETTER(NrOfSatellites, m_nrOfSatellites)
    GETTER(FixQuality, m_fixQuality)
};
}  // namespace vfrb::object
