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

#include "GpsPosition.h"

namespace object
{
GpsPosition::GpsPosition() : Object()
{}

GpsPosition::GpsPosition(std::uint32_t vPriority) : Object(vPriority)
{}

GpsPosition::GpsPosition(const Position& crPosition, double vGeoid)
    : Object(), m_position(crPosition), m_geoid(vGeoid)
{}

GpsPosition::~GpsPosition() noexcept
{}

void GpsPosition::assign(Object&& rvOther)
{
    Object::assign(std::move(rvOther));
    GpsPosition&& rvUpdate = static_cast<GpsPosition&&>(rvOther);
    this->m_position        = rvUpdate.m_position;
    this->m_timeStamp       = rvUpdate.m_timeStamp;
    this->m_nrOfSatellites  = rvUpdate.m_nrOfSatellites;
    this->m_fixQuality      = rvUpdate.m_fixQuality;
    this->m_geoid           = rvUpdate.m_geoid;
    this->m_dilution        = rvUpdate.m_dilution;
}

bool GpsPosition::canUpdate(const Object& crOther) const
{
    const GpsPosition& crToUpdate = static_cast<const GpsPosition&>(crOther);
    return (this->m_timeStamp > crToUpdate.m_timeStamp) && Object::canUpdate(crOther);
}

}  // namespace object
