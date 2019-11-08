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

#include "data/AtmosphereData.h"

using namespace vfrb::object;
using namespace vfrb::concurrent;

namespace vfrb::data
{
AtmosphereData::AtmosphereData(AccessFn&& fn) : Data(std::move(fn)) {}

AtmosphereData::AtmosphereData(AccessFn&& fn, CAtmosphere const& atmosphere)
    : Data(std::move(fn)), m_atmosphere(atmosphere)
{}

void AtmosphereData::access()
{
    LockGuard lk(m_mutex);
    m_accessFn({++m_atmosphere, {*m_atmosphere}});
}

bool AtmosphereData::update(CObject&& atmosphere)
{
    LockGuard lk(m_mutex);
    return m_atmosphere.TryUpdate(std::move(atmosphere));
}

auto AtmosphereData::atmPressure() const -> decltype(m_atmosphere.pressure())
{
    LockGuard lk(m_mutex);
    return m_atmosphere.pressure();
}
}  // namespace vfrb::data
