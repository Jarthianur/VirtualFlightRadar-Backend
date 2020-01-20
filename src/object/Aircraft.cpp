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

#include "object/Aircraft.h"

#include <typeinfo>

#include "util/utility.hpp"

namespace vfrb::object
{
CAircraft::CAircraft(u32 prio_, std::string_view const& id_, EIdType idT_, EAircraftType aT_,
                     SLocation const& loc_, SMovement const& move_, CTimestamp const& ts_)
    : CObject(prio_),
      m_id(id_),
      m_idType(idT_ > EIdType::OGN ? EIdType::RANDOM : idT_),
      m_aircraftType(aT_ > EAircraftType::STATIC_OBJECT ? EAircraftType::UNKNOWN : aT_),
      m_targetType(ETargetType::FLARM),
      m_location(loc_),
      m_movement(move_),
      m_timestamp(ts_),
      m_fullInfo(true)
{
    util::FailOutsideBounds(m_location.Latitude, SLocation::MIN_LATITUDE, SLocation::MAX_LATITUDE);
    util::FailOutsideBounds(m_location.Longitude, SLocation::MIN_LONGITUDE, SLocation::MAX_LONGITUDE);
}

CAircraft::CAircraft(u32 prio_, std::string_view const& id_, EIdType idT_, EAircraftType aT_,
                     SLocation const& loc_, CTimestamp const& ts_)
    : CAircraft(prio_, id_, idT_, aT_, loc_, {.0, .0, .0}, ts_)
{
    m_targetType = ETargetType::TRANSPONDER;
    m_fullInfo   = false;
}

CAircraft::CAircraft(CAircraft&& other_) : m_id(other_.m_id)
{
    assign(std::move(other_));
}

void CAircraft::assign(CObject&& other_)
{
    try
    {
        auto&& other = dynamic_cast<CAircraft&&>(other_);
        CObject::assign(std::move(other_));
        this->m_idType       = other.m_idType;
        this->m_aircraftType = other.m_aircraftType;
        this->m_targetType   = other.m_targetType;
        this->m_location     = other.m_location;
        this->m_movement     = other.m_movement;
        this->m_timestamp    = other.m_timestamp;
        this->m_fullInfo     = other.m_fullInfo;
    }
    catch ([[maybe_unused]] std::bad_cast const&)
    {}
}

bool CAircraft::canUpdate(CObject const& other_) const
{
    try
    {
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
    }
    catch ([[maybe_unused]] std::bad_cast const&)
    {
        return false;
    }
}

CAircraft& CAircraft::operator=(CAircraft&& other_)
{
    TryUpdate(std::move(other_));
    return *this;
}

auto CAircraft::IdType() const -> decltype(m_idType)
{
    return m_idType;
}

auto CAircraft::AircraftType() const -> decltype(m_aircraftType)
{
    return m_aircraftType;
}

auto CAircraft::Id() const -> decltype(m_id) const&
{
    return m_id;
}

auto CAircraft::TargetType() const -> decltype(m_targetType)
{
    return m_targetType;
}

auto CAircraft::Location() const -> decltype(m_location) const&
{
    return m_location;
}

auto CAircraft::Movement() const -> decltype(m_movement) const&
{
    return m_movement;
}

auto CAircraft::Timestamp() const -> decltype(m_timestamp) const&
{
    return m_timestamp;
}

auto CAircraft::HasFullInfo() const -> decltype(m_fullInfo)
{
    return m_fullInfo;
}

void CAircraft::TargetType(ETargetType tt)
{
    m_targetType = tt;
}
}  // namespace vfrb::object
