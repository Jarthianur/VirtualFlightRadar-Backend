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

#include "data/WindData.h"

using namespace vfrb::object;
using namespace vfrb::concurrent;

namespace vfrb::data
{
CWindData::CWindData(AccessFn&& fn) : IData(std::move(fn)) {}

CWindData::CWindData(AccessFn&& fn, object::CWind const& wind) : IData(std::move(fn)), m_wind(wind) {}

bool CWindData::Update(CObject&& wind)
{
    LockGuard lk(m_mutex);
    return m_wind.TryUpdate(std::move(wind));
}

void CWindData::Access()
{
    LockGuard lk(m_mutex);
    m_accessFn({++m_wind, {*m_wind}});
    (*m_wind).clear();
}
}  // namespace vfrb::data
