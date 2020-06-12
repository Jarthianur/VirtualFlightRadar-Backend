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

#include "object/CGpsPosition.hpp"

#include <typeinfo>
#include <utility>

#include "util/Bounds.hpp"

using vfrb::util::FailOutsideBounds;

namespace vfrb::object
{
CGpsPosition::CGpsPosition(u32 prio_, SLocation const& loc_, f64 geo_)
    : CGpsPosition(prio_, loc_, geo_, 1.0, 3, 5, CTimestamp()) {}

CGpsPosition::CGpsPosition(u32 prio_, SLocation const& loc_, f64 geo_, f64 dil_, u8 sat_, s8 qual_,
                           CTimestamp const& ts_)
    : CObject(prio_),
      m_location(loc_),
      m_geoid(geo_),
      m_dilution(dil_),
      m_nrOfSatellites(sat_),
      m_fixQuality(qual_),
      m_timestamp(ts_) {
    FailOutsideBounds(m_location.Latitude, SLocation::MIN_LATITUDE, SLocation::MAX_LATITUDE);
    FailOutsideBounds(m_location.Longitude, SLocation::MIN_LONGITUDE, SLocation::MAX_LONGITUDE);
    FailOutsideBounds(m_location.Altitude, SLocation::MIN_ALTITUDE, SLocation::MAX_ALTITUDE);
}

void CGpsPosition::assign(CObject&& other_) {
    try {
        auto&& other = dynamic_cast<CGpsPosition&&>(other_);
        CObject::assign(std::move(other_));
        this->m_location       = other.m_location;
        this->m_timestamp      = other.m_timestamp;
        this->m_nrOfSatellites = other.m_nrOfSatellites;
        this->m_fixQuality     = other.m_fixQuality;
        this->m_geoid          = other.m_geoid;
        this->m_dilution       = other.m_dilution;
    } catch ([[maybe_unused]] std::bad_cast const&) {
    }
}

auto CGpsPosition::canUpdate(CObject const& other_) const -> bool {
    try {
        auto const& other = dynamic_cast<const CGpsPosition&>(other_);
        return (this->m_timestamp > other.m_timestamp) && CObject::canUpdate(other_);
    } catch ([[maybe_unused]] std::bad_cast const&) {
        return false;
    }
}
}  // namespace vfrb::object
