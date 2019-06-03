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

#include "object/GpsPosition.h"

#include <typeinfo>
#include <utility>

namespace object
{
GpsPosition::GpsPosition() : Object() {}

GpsPosition::GpsPosition(std::uint32_t priority) : Object(priority) {}

GpsPosition::GpsPosition(std::uint32_t priority, const Location& location, double geoid, double dilution,
                         std::uint8_t satellites, std::int8_t quality,
                         const Timestamp<time::DateTimeImplBoost>& timestamp)
    : Object(priority),
      m_location(location),
      m_geoid(geoid),
      m_dilution(dilution),
      m_nrOfSatellites(satellites),
      m_fixQuality(quality),
      m_timestamp(timestamp)
{}

void GpsPosition::assign(Object&& other)
{
    try
    {
        GpsPosition&& update = dynamic_cast<GpsPosition&&>(other);
        Object::assign(std::move(other));
        this->m_location       = update.m_location;
        this->m_timestamp      = update.m_timestamp;
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
        return (this->m_timestamp > toUpdate.m_timestamp) && Object::canUpdate(other);
    }
    catch (const std::bad_cast&)
    {
        return false;
    }
}

util::CStringPack GpsPosition::getNMEA() const
{
    return m_nmea;
}

util::CString<GpsPosition::NMEA_SIZE>& GpsPosition::operator*()
{
    return m_nmea;
}

auto GpsPosition::getLocation() const -> const decltype(m_location)&
{
    return m_location;
}

auto GpsPosition::getGeoid() const -> decltype(m_geoid)
{
    return m_geoid;
}

auto GpsPosition::getTimestamp() const -> const decltype(m_timestamp)&
{
    return m_timestamp;
}

auto GpsPosition::getDilution() const -> decltype(m_dilution)
{
    return m_dilution;
}

auto GpsPosition::getNrOfSatellites() const -> decltype(m_nrOfSatellites)
{
    return m_nrOfSatellites;
}

auto GpsPosition::getFixQuality() const -> decltype(m_fixQuality)
{
    return m_fixQuality;
}

}  // namespace object
