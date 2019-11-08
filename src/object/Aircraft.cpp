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
CAircraft::CAircraft(u32 priority, std::string_view const& id, EIdType idT, EAircraftType aT, SLocation const& loc,
                   SMovement const& move, CTimestamp const& timestamp)
    : CObject(priority),
      m_id(id),
      m_idType(idT > EIdType::OGN ? EIdType::RANDOM : idT),
      m_aircraftType(aT > EAircraftType::STATIC_OBJECT ? EAircraftType::UNKNOWN : aT),
      m_targetType(ETargetType::FLARM),
      m_location(loc),
      m_movement(move),
      m_timestamp(timestamp),
      m_fullInfo(true)
{
    util::CheckLimits(m_location.Latitude, SLocation::MIN_LATITUDE, SLocation::MAX_LATITUDE);
    util::CheckLimits(m_location.Longitude, SLocation::MIN_LONGITUDE, SLocation::MAX_LONGITUDE);
}

CAircraft::CAircraft(u32 priority, std::string_view const& id, EIdType idT, EAircraftType aT, SLocation const& loc,
                   CTimestamp const& timestamp)
    : CAircraft(priority, id, idT, aT, loc, {.0, .0, .0}, timestamp)
{
    m_targetType = ETargetType::TRANSPONDER;
    m_fullInfo   = false;
}

CAircraft::CAircraft(CAircraft&& other) : m_id(other.m_id)
{
    assign(std::move(other));
}

void CAircraft::assign(CObject&& other)
{
    try
    {
        auto&& update = dynamic_cast<CAircraft&&>(other);
        CObject::assign(std::move(other));
        this->m_idType       = update.m_idType;
        this->m_aircraftType = update.m_aircraftType;
        this->m_targetType   = update.m_targetType;
        this->m_location     = update.m_location;
        this->m_movement     = update.m_movement;
        this->m_timestamp    = update.m_timestamp;
        this->m_fullInfo     = update.m_fullInfo;
    }
    catch (std::bad_cast const&)
    {}
}

bool CAircraft::canUpdate(CObject const& other) const
{
    try
    {
        auto const& toUpdate = dynamic_cast<CAircraft const&>(other);
        return (this->m_timestamp > toUpdate.m_timestamp) &&
               (toUpdate.m_targetType == ETargetType::TRANSPONDER ||
                this->m_targetType == ETargetType::FLARM) &&
               CObject::canUpdate(other);
    }
    catch (std::bad_cast const&)
    {
        return false;
    }
}

CAircraft& CAircraft::operator=(CAircraft&& other)
{
    TryUpdate(std::move(other));
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
