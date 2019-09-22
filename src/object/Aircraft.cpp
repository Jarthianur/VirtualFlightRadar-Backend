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
Aircraft::Aircraft(u32 priority, std::string_view const& id, IdType idT, AircraftType aT, Location const& loc,
                   Movement const& move, Timestamp const& timestamp)
    : Object(priority),
      m_id(id),
      m_idType(idT > IdType::OGN ? IdType::RANDOM : idT),
      m_aircraftType(aT > AircraftType::STATIC_OBJECT ? AircraftType::UNKNOWN : aT),
      m_targetType(TargetType::FLARM),
      m_location(loc),
      m_movement(move),
      m_timestamp(timestamp),
      m_fullInfo(true)
{
    util::checkLimits(m_location.latitude, Location::MIN_LATITUDE, Location::MAX_LATITUDE);
    util::checkLimits(m_location.longitude, Location::MIN_LONGITUDE, Location::MAX_LONGITUDE);
}

Aircraft::Aircraft(u32 priority, std::string_view const& id, IdType idT, AircraftType aT, Location const& loc,
                   Timestamp const& timestamp)
    : Aircraft(priority, id, idT, aT, loc, {.0, .0, .0}, timestamp)
{
    m_targetType = TargetType::TRANSPONDER;
    m_fullInfo   = false;
}

Aircraft::Aircraft(Aircraft&& other) : m_id(other.m_id)
{
    assign(std::move(other));
}

void Aircraft::assign(Object&& other)
{
    try
    {
        auto&& update = dynamic_cast<Aircraft&&>(other);
        Object::assign(std::move(other));
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

bool Aircraft::canUpdate(Object const& other) const
{
    try
    {
        auto const& toUpdate = dynamic_cast<Aircraft const&>(other);
        return (this->m_timestamp > toUpdate.m_timestamp) &&
               (toUpdate.m_targetType == TargetType::TRANSPONDER ||
                this->m_targetType == TargetType::FLARM) &&
               Object::canUpdate(other);
    }
    catch (std::bad_cast const&)
    {
        return false;
    }
}

std::string_view Aircraft::nmea() const
{
    return m_nmea;
}

util::CString<Aircraft::NMEA_SIZE>& Aircraft::operator*()
{
    return m_nmea;
}

Aircraft& Aircraft::operator=(Aircraft&& other)
{
    tryUpdate(std::move(other));
    return *this;
}

auto Aircraft::idType() const -> decltype(m_idType)
{
    return m_idType;
}

auto Aircraft::aircraftType() const -> decltype(m_aircraftType)
{
    return m_aircraftType;
}

auto Aircraft::id() const -> decltype(m_id) const&
{
    return m_id;
}

auto Aircraft::targetType() const -> decltype(m_targetType)
{
    return m_targetType;
}

auto Aircraft::location() const -> decltype(m_location) const&
{
    return m_location;
}

auto Aircraft::movement() const -> decltype(m_movement) const&
{
    return m_movement;
}

auto Aircraft::timestamp() const -> decltype(m_timestamp) const&
{
    return m_timestamp;
}

auto Aircraft::hasFullInfo() const -> decltype(m_fullInfo)
{
    return m_fullInfo;
}

void Aircraft::targetType(TargetType tt)
{
    m_targetType = tt;
}
}  // namespace vfrb::object
