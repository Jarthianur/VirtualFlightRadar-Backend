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

#include "Aircraft.h"

namespace object
{
Aircraft::Aircraft() : Aircraft(0)
{}

Aircraft::Aircraft(std::uint32_t vPriority)
    : Object(vPriority),
      m_idType(IdType::ICAO),
      m_aircraftType(AircraftType::POWERED_AIRCRAFT),
      m_targetType(TargetType::TRANSPONDER)
{}

Aircraft::~Aircraft() noexcept
{}

void Aircraft::assign(Object&& rvOther)
{
    Object::assign(std::move(rvOther));
    Aircraft&& rvUpdate  = static_cast<Aircraft&&>(rvOther);
    this->m_idType       = rvUpdate.m_idType;
    this->m_aircraftType = rvUpdate.m_aircraftType;
    this->m_targetType   = rvUpdate.m_targetType;
    this->m_position     = rvUpdate.m_position;
    this->m_movement     = rvUpdate.m_movement;
    this->m_timeStamp    = rvUpdate.m_timeStamp;
    this->m_fullInfo     = rvUpdate.m_fullInfo;
}

bool Aircraft::canUpdate(const Object& crOther) const
{
    const Aircraft& crToUpdate = static_cast<const Aircraft&>(crOther);
    return (this->m_timeStamp > crToUpdate.m_timeStamp)
           && (crToUpdate.m_targetType == TargetType::TRANSPONDER
               || this->m_targetType == TargetType::FLARM)
           && Object::canUpdate(crOther);
}

void Aircraft::set_aircraftType(Aircraft::AircraftType vType)
{
    m_aircraftType = vType < AircraftType::UNKNOWN || vType > AircraftType::STATIC_OBJECT
                         ? AircraftType::UNKNOWN
                         : vType;
}

void Aircraft::set_idType(Aircraft::IdType vType)
{
    m_idType = vType < IdType::UNRECOGNIZED || vType > IdType::OGN ? IdType::UNRECOGNIZED : vType;
}

}  // namespace object
