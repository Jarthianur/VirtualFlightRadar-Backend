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

#include "util/utility.hpp"

namespace vfrb::object
{
GpsPosition::GpsPosition(u32 priority, Location const& location, f64 geoid)
    : GpsPosition(priority, location, geoid, 1.0, 3, 5, Timestamp())
{}

GpsPosition::GpsPosition(u32 priority, Location const& location, f64 geoid, f64 dilution, u8 satellites,
                         s8 quality, Timestamp const& timestamp)
    : Object(priority),
      m_location(location),
      m_geoid(geoid),
      m_dilution(dilution),
      m_nrOfSatellites(satellites),
      m_fixQuality(quality),
      m_timestamp(timestamp)
{
    util::checkLimits(m_location.latitude, Location::MIN_LATITUDE, Location::MAX_LATITUDE);
    util::checkLimits(m_location.longitude, Location::MIN_LONGITUDE, Location::MAX_LONGITUDE);
    util::checkLimits(m_location.altitude, Location::MIN_ALTITUDE, Location::MAX_ALTITUDE);
}

void GpsPosition::assign(Object&& other)
{
    try
    {
        auto&& update = dynamic_cast<GpsPosition&&>(other);
        Object::assign(std::move(other));
        this->m_location       = update.m_location;
        this->m_timestamp      = update.m_timestamp;
        this->m_nrOfSatellites = update.m_nrOfSatellites;
        this->m_fixQuality     = update.m_fixQuality;
        this->m_geoid          = update.m_geoid;
        this->m_dilution       = update.m_dilution;
    }
    catch (std::bad_cast const&)
    {}
}

bool GpsPosition::canUpdate(Object const& other) const
{
    try
    {
        auto const& toUpdate = dynamic_cast<const GpsPosition&>(other);
        return (this->m_timestamp > toUpdate.m_timestamp) && Object::canUpdate(other);
    }
    catch (std::bad_cast const&)
    {
        return false;
    }
}

auto GpsPosition::location() const -> decltype(m_location) const&
{
    return m_location;
}

auto GpsPosition::geoid() const -> decltype(m_geoid)
{
    return m_geoid;
}

auto GpsPosition::timestamp() const -> decltype(m_timestamp) const&
{
    return m_timestamp;
}

auto GpsPosition::dilution() const -> decltype(m_dilution)
{
    return m_dilution;
}

auto GpsPosition::nrOfSatellites() const -> decltype(m_nrOfSatellites)
{
    return m_nrOfSatellites;
}

auto GpsPosition::fixQuality() const -> decltype(m_fixQuality)
{
    return m_fixQuality;
}
}  // namespace vfrb::object
