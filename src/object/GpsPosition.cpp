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

#include <typeinfo>
#include <utility>

#include "object/CGpsPosition.hpp"
#include "util/bounds.hpp"

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
    util::FailOutsideBounds(m_location.Latitude, SLocation::MIN_LATITUDE, SLocation::MAX_LATITUDE);
    util::FailOutsideBounds(m_location.Longitude, SLocation::MIN_LONGITUDE, SLocation::MAX_LONGITUDE);
    util::FailOutsideBounds(m_location.Altitude, SLocation::MIN_ALTITUDE, SLocation::MAX_ALTITUDE);
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

bool CGpsPosition::canUpdate(CObject const& other_) const {
    try {
        auto const& other = dynamic_cast<const CGpsPosition&>(other_);
        return (this->m_timestamp > other.m_timestamp) && CObject::canUpdate(other_);
    } catch ([[maybe_unused]] std::bad_cast const&) {
        return false;
    }
}
}  // namespace vfrb::object
