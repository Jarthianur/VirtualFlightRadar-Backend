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

#include "object/CObject.hpp"

#include <utility>

namespace vfrb::object
{
CObject::CObject(u32 prio_) : m_lastPriority(prio_) {}

void CObject::assign(CObject&& other_) {
    this->m_lastPriority = other_.m_lastPriority;
    this->m_updateAge    = 0;
}

auto CObject::TryUpdate(CObject&& other_) -> bool {
    if (other_.canUpdate(*this)) {
        this->assign(std::move(other_));
        return true;
    }
    return false;
}

auto CObject::canUpdate(CObject const& other_) const -> bool {
    return this->m_lastPriority >= other_.m_lastPriority || other_.m_updateAge >= OUTDATED;
}

auto CObject::operator++() -> CObject& {
    ++m_updateAge;
    return *this;
}
}  // namespace vfrb::object