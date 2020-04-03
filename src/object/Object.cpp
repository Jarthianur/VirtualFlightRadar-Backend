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

#include <utility>

#include "object/CObject.hpp"

namespace vfrb::object
{
CObject::CObject(u32 prio_) : m_lastPriority(prio_) {}

void CObject::assign(CObject&& other_) {
    this->m_lastPriority = other_.m_lastPriority;
    this->m_updateAge    = 0;
}

bool CObject::TryUpdate(CObject&& other_) {
    if (other_.canUpdate(*this)) {
        this->assign(std::move(other_));
        return true;
    }
    return false;
}

bool CObject::canUpdate(CObject const& other_) const {
    return this->m_lastPriority >= other_.m_lastPriority || other_.m_updateAge >= OUTDATED;
}

CObject& CObject::operator++() {
    ++m_updateAge;
    return *this;
}

auto CObject::UpdateAge() const -> decltype(m_updateAge) {
    return m_updateAge;
}
}  // namespace vfrb::object
