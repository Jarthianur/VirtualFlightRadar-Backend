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

using namespace object;

namespace data
{
AtmosphereData::AtmosphereData() : Data() {}

AtmosphereData::AtmosphereData(const Atmosphere& atmosphere) : Data(), m_atmosphere(atmosphere) {}

void AtmosphereData::get_serialized(std::string& dest)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    dest += (++m_atmosphere).get_serialized();
}

bool AtmosphereData::update(Object&& atmosphere)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_atmosphere.tryUpdate(std::move(atmosphere));
}

double AtmosphereData::get_atmPressure()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_atmosphere.get_pressure();
}

}  // namespace data
