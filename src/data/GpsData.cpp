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

#include "data/GpsData.h"

using namespace vfrb::object;

namespace vfrb::data
{
GpsData::GpsData(AccessFn&& fn, GpsPosition const& position, bool ground)
    : Data(std::move(fn)), m_position(position), m_groundMode(ground)
{
    m_processor.process(m_position);
}

void GpsData::access()
{
    std::lock_guard lk(m_mutex);
    try
    {
        m_processor.process(m_position);
        m_accessFn(++m_position);
    }
    catch (vfrb::error::Exception const&)
    {}
}

bool GpsData::update(Object&& position)
{
    std::lock_guard lk(m_mutex);
    if (m_positionLocked)
    {
        throw error::PositionAlreadyLocked();
    }
    bool updated = m_position.tryUpdate(std::move(position));
    if (updated)
    {
        m_processor.process(m_position);
        if (m_groundMode && isPositionGood())
        {
            throw error::ReceivedGoodPosition();
        }
    }
    return updated;
}

auto GpsData::location() const -> decltype(m_position.location())
{
    std::lock_guard lk(m_mutex);
    return m_position.location();
}

bool GpsData::isPositionGood() const
{
    return m_position.nrOfSatellites() >= GPS_NR_SATS_GOOD && m_position.fixQuality() >= GPS_FIX_GOOD &&
           m_position.dilution() <= GPS_HOR_DILUTION_GOOD;
}

namespace error
{
char const* PositionAlreadyLocked::what() const noexcept
{
    return "position was locked before";
}

char const* ReceivedGoodPosition::what() const noexcept
{
    return "received good position";
}
}  // namespace error
}  // namespace vfrb::data
