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

#include <typeinfo>

namespace object
{
GpsPosition::GpsPosition() : Object() {}

GpsPosition::GpsPosition(std::uint32_t priority) : Object(priority) {}

GpsPosition::GpsPosition(const Position& position, double geoid)
    : Object(), m_position(position), m_geoid(geoid)
{}

GpsPosition::~GpsPosition() noexcept {}

void GpsPosition::assign(Object&& other)
{
    try
    {
        GpsPosition&& update = dynamic_cast<GpsPosition&&>(other);
        Object::assign(std::move(other));
        this->m_position       = update.m_position;
        this->m_timeStamp      = update.m_timeStamp;
        this->m_nrOfSatellites = update.m_nrOfSatellites;
        this->m_fixQuality     = update.m_fixQuality;
        this->m_geoid          = update.m_geoid;
        this->m_dilution       = update.m_dilution;
    }
    catch (const std::bad_cast&)
    {}
}

bool GpsPosition::canUpdate(const Object& other) const
{
    try
    {
        const GpsPosition& toUpdate = dynamic_cast<const GpsPosition&>(other);
        return (this->m_timeStamp > toUpdate.m_timeStamp) && Object::canUpdate(other);
    }
    catch (const std::bad_cast&)
    {
        return false;
    }
}

}  // namespace object
