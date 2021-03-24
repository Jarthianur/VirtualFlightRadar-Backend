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

#include "object/CAircraft.hpp"

#include "util/Bounds.hpp"

using vfrb::util::FailOutsideBounds;

namespace vfrb::object
{
CAircraft::CAircraft(u32 prio_, StringView const& id_, EIdType idT_, EAircraftType aT_, ETargetType tt_,
                     SLocation const& loc_, SMovement const& move_, CTimestamp const& ts_)
    : CObject(prio_),
      m_id(id_),
      m_idType(idT_ > EIdType::OGN ? EIdType::RANDOM : idT_),
      m_aircraftType(aT_ > EAircraftType::STATIC_OBJECT ? EAircraftType::UNKNOWN : aT_),
      m_targetType(tt_),
      m_location(loc_),
      m_movement(move_),
      m_timestamp(ts_) {
    FailOutsideBounds(m_location.Latitude, SLocation::MIN_LATITUDE, SLocation::MAX_LATITUDE);
    FailOutsideBounds(m_location.Longitude, SLocation::MIN_LONGITUDE, SLocation::MAX_LONGITUDE);
}

void
CAircraft::assign(CObject&& other_) {
    try {
        auto&& other = dynamic_cast<CAircraft&&>(other_);
        CObject::assign(std::move(other_));
        this->m_idType       = other.m_idType;
        this->m_aircraftType = other.m_aircraftType;
        this->m_targetType   = other.m_targetType;
        this->m_location     = other.m_location;
        this->m_movement     = other.m_movement;
        this->m_timestamp    = other.m_timestamp;
    } catch ([[maybe_unused]] std::bad_cast const&) {
    }
}

auto
CAircraft::canUpdate(CObject const& other_) const -> bool {
    try {
        auto const& other = dynamic_cast<CAircraft const&>(other_);
        // timestamp must always be more up to date (1)
        // other aircraft is outdated anyway (2)
        // FLARM can always update TRANSPONDER if (1) holds
        // equal types require at least equal priority
        // TRANSPONDER can only update FLARM if (2) holds
        return (this->m_timestamp > other.m_timestamp) &&
               (other.m_updateAge >= OUTDATED ||
                (other.m_targetType == ETargetType::TRANSPONDER &&
                 this->m_targetType == ETargetType::FLARM) ||
                (other.m_targetType == this->m_targetType && this->m_lastPriority >= other.m_lastPriority));
    } catch ([[maybe_unused]] std::bad_cast const&) {
        return false;
    }
}

void
CAircraft::TargetType(ETargetType tt) {
    m_targetType = tt;
}
}  // namespace vfrb::object
