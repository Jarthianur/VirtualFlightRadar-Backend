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

#include "object/Atmosphere.h"

#include <typeinfo>
#include <utility>

#include "util/math.hpp"

namespace object
{
Atmosphere::Atmosphere() : Object() {}

Atmosphere::Atmosphere(u32 priority) : Object(priority) {}

Atmosphere::Atmosphere(u32 priority, f64 pressure) : Object(priority), m_pressure(pressure)
{
    math::checkLimits(m_pressure, MIN_PRESSURE, MAX_PRESSURE);
}

void Atmosphere::assign(Object&& other)
{
    try
    {
        auto&& update = dynamic_cast<Atmosphere&&>(other);
        Object::assign(std::move(other));
        this->m_pressure = update.m_pressure;
    }
    catch (std::bad_cast const&)
    {}
}

util::CStringPack Atmosphere::nmea() const
{
    return {m_nmea.c_str(), m_nmea.size()};
}

auto Atmosphere::pressure() const -> decltype(m_pressure)
{
    return m_pressure;
}

str& Atmosphere::operator*()
{
    return m_nmea;
}
}  // namespace object
