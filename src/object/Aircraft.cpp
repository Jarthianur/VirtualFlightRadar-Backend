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

namespace object
{
Aircraft::Aircraft(std::uint32_t priority, const std::string& id, IdType idT, AircraftType aT,
                   const Location& loc, const Movement& move,
                   const Timestamp<time::DateTimeImplBoost>& timestamp)
    : Object(priority),
      m_id(id),
      m_idType(idT > IdType::OGN ? IdType::RANDOM : idT),
      m_aircraftType(aT > AircraftType::STATIC_OBJECT ? AircraftType::UNKNOWN : aT),
      m_targetType(TargetType::FLARM),
      m_location(loc),
      m_movement(move),
      m_timestamp(timestamp),
      m_fullInfo(true)
{}

Aircraft::Aircraft(std::uint32_t priority, const std::string& id, IdType idT, AircraftType aT,
                   const Location& loc, const Timestamp<time::DateTimeImplBoost>& timestamp)
    : Object(priority),
      m_id(id),
      m_idType(idT > IdType::OGN ? IdType::RANDOM : idT),
      m_aircraftType(aT > AircraftType::STATIC_OBJECT ? AircraftType::UNKNOWN : aT),
      m_targetType(TargetType::TRANSPONDER),
      m_location(loc),
      m_timestamp(timestamp),
      m_fullInfo(false)
{}

void Aircraft::assign(Object&& other)
{
    try
    {
        Aircraft&& update = dynamic_cast<Aircraft&&>(other);
        Object::assign(std::move(other));
        this->m_idType       = update.m_idType;
        this->m_aircraftType = update.m_aircraftType;
        this->m_targetType   = update.m_targetType;
        this->m_location     = update.m_location;
        this->m_movement     = update.m_movement;
        this->m_timestamp    = update.m_timestamp;
        this->m_fullInfo     = update.m_fullInfo;
    }
    catch (const std::bad_cast&)
    {}
}

bool Aircraft::canUpdate(const Object& other) const
{
    try
    {
        const Aircraft& toUpdate = dynamic_cast<const Aircraft&>(other);
        return (this->m_timestamp > toUpdate.m_timestamp) &&
               (toUpdate.m_targetType == TargetType::TRANSPONDER ||
                this->m_targetType == TargetType::FLARM) &&
               Object::canUpdate(other);
    }
    catch (const std::bad_cast&)
    {
        return false;
    }
}

util::CStringPack Aircraft::getNMEA() const
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

auto Aircraft::getIdType() const -> decltype(m_idType)
{
    return m_idType;
}

auto Aircraft::getAircraftType() const -> decltype(m_aircraftType)
{
    return m_aircraftType;
}

auto Aircraft::getId() const -> const decltype(m_id)&
{
    return m_id;
}

auto Aircraft::getTargetType() const -> decltype(m_targetType)
{
    return m_targetType;
}

auto Aircraft::getLocation() const -> const decltype(m_location)&
{
    return m_location;
}

auto Aircraft::getMovement() const -> const decltype(m_movement)&
{
    return m_movement;
}

auto Aircraft::getTimestamp() const -> const decltype(m_timestamp)&
{
    return m_timestamp;
}

auto Aircraft::hasFullInfo() const -> decltype(m_fullInfo)
{
    return m_fullInfo;
}

void Aircraft::setTargetType(TargetType tt)
{
    m_targetType = tt;
}

}  // namespace object
