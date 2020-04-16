/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "object/CAtmosphere.hpp"

#include <typeinfo>
#include <utility>

#include "util/bounds.hpp"

namespace vfrb::object
{
CAtmosphere::CAtmosphere(u32 prio_, String&& nmea_) : CObject(prio_), m_nmea(std::move(nmea_)) {}

CAtmosphere::CAtmosphere(CAtmosphere&& other_) noexcept
    : CObject(other_), m_pressure(other_.m_pressure), m_nmea(std::move(other_.m_nmea)) {}

CAtmosphere& CAtmosphere::operator=(CAtmosphere&& other_) noexcept {
    CObject::operator=(other_);
    m_pressure       = other_.m_pressure;
    m_nmea           = std::move(other_.m_nmea);
    return *this;
}

CAtmosphere::CAtmosphere(u32 prio_, f64 press_, String&& nmea_)
    : CObject(prio_), m_pressure(press_), m_nmea(std::move(nmea_)) {
    util::FailOutsideBounds(m_pressure, MIN_PRESSURE, MAX_PRESSURE);
}

void CAtmosphere::assign(CObject&& other_) {
    try {
        auto&& other = dynamic_cast<CAtmosphere&&>(other_);
        CObject::assign(std::move(other_));
        this->m_pressure = other.m_pressure;
        this->m_nmea     = std::move(other.m_nmea);
    } catch ([[maybe_unused]] std::bad_cast const&) {
    }
}
}  // namespace vfrb::object
