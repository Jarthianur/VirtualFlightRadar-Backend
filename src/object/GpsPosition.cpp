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
CGpsPosition::CGpsPosition(u32 priority, SLocation const& location, f64 geoid)
    : CGpsPosition(priority, location, geoid, 1.0, 3, 5, CTimestamp())
{}

CGpsPosition::CGpsPosition(u32 priority, SLocation const& location, f64 geoid, f64 dilution, u8 satellites,
                         s8 quality, CTimestamp const& timestamp)
    : CObject(priority),
      m_location(location),
      m_geoid(geoid),
      m_dilution(dilution),
      m_nrOfSatellites(satellites),
      m_fixQuality(quality),
      m_timestamp(timestamp)
{
    util::CheckLimits(m_location.Latitude, SLocation::MIN_LATITUDE, SLocation::MAX_LATITUDE);
    util::CheckLimits(m_location.Longitude, SLocation::MIN_LONGITUDE, SLocation::MAX_LONGITUDE);
    util::CheckLimits(m_location.Altitude, SLocation::MIN_ALTITUDE, SLocation::MAX_ALTITUDE);
}

void CGpsPosition::assign(CObject&& other)
{
    try
    {
        auto&& update = dynamic_cast<CGpsPosition&&>(other);
        CObject::assign(std::move(other));
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

bool CGpsPosition::canUpdate(CObject const& other) const
{
    try
    {
        auto const& toUpdate = dynamic_cast<const CGpsPosition&>(other);
        return (this->m_timestamp > toUpdate.m_timestamp) && CObject::canUpdate(other);
    }
    catch (std::bad_cast const&)
    {
        return false;
    }
}

auto CGpsPosition::Location() const -> decltype(m_location) const&
{
    return m_location;
}

auto CGpsPosition::Geoid() const -> decltype(m_geoid)
{
    return m_geoid;
}

auto CGpsPosition::Timestamp() const -> decltype(m_timestamp) const&
{
    return m_timestamp;
}

auto CGpsPosition::Dilution() const -> decltype(m_dilution)
{
    return m_dilution;
}

auto CGpsPosition::NrOfSatellites() const -> decltype(m_nrOfSatellites)
{
    return m_nrOfSatellites;
}

auto CGpsPosition::FixQuality() const -> decltype(m_fixQuality)
{
    return m_fixQuality;
}
}  // namespace vfrb::object
