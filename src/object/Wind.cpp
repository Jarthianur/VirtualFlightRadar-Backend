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

#include "object/CWind.hpp"

namespace vfrb::object
{
CWind::CWind(u32 prio_, String&& nmea_) : CObject(prio_), m_nmea(std::move(nmea_)) {}

CWind::CWind(CWind&& other_) noexcept : CObject(other_), m_nmea(std::move(other_.m_nmea)) {}

CWind& CWind::operator=(CWind&& other_) noexcept {
    CObject::operator=(other_);
    m_nmea           = std::move(other_.m_nmea);
    return *this;
}

void CWind::Clear() {
    m_nmea.clear();
}

void CWind::assign(CObject&& other_) {
    try {
        auto&& other = dynamic_cast<CWind&&>(other_);
        CObject::assign(std::move(other_));
        this->m_nmea = std::move(other.m_nmea);
    } catch ([[maybe_unused]] std::bad_cast const&) {
    }
}

auto CWind::Nmea() const -> decltype(m_nmea) const& {
    return m_nmea;
}
}  // namespace vfrb::object
