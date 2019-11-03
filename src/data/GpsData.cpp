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
    : Data(std::move(fn)), m_position(std::make_tuple(position, "")), m_groundMode(ground)
{
    auto& [pos, cstr] = m_position;
    m_processor.process(pos, cstr);
}

void GpsData::access()
{
    std::lock_guard lk(m_mutex);
    try
    {
        auto& [pos, cstr] = m_position;
        m_processor.process(pos, cstr);
        m_accessFn({++pos, cstr});
    }
    catch ([[maybe_unused]] vfrb::error::Error const&)
    {}
}

bool GpsData::update(Object&& position)
{
    std::lock_guard lk(m_mutex);
    if (m_positionLocked)
    {
        throw error::PositionAlreadyLocked();
    }
    auto& [pos, cstr] = m_position;
    bool updated      = pos.tryUpdate(std::move(position));
    if (updated)
    {
        m_processor.process(pos, cstr);
        if (m_groundMode && isPositionGood())
        {
            throw error::ReceivedGoodPosition();
        }
    }
    return updated;
}

auto GpsData::location() const -> decltype(std::get<0>(m_position).location())
{
    std::lock_guard lk(m_mutex);
    return std::get<0>(m_position).location();
}

bool GpsData::isPositionGood() const
{
    auto const& pos = std::get<0>(m_position);
    return pos.nrOfSatellites() >= GPS_NR_SATS_GOOD && pos.fixQuality() >= GPS_FIX_GOOD &&
           pos.dilution() <= GPS_HOR_DILUTION_GOOD;
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
