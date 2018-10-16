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

#include "Object.h"

namespace object
{
Object::Object() : m_lastPriority(0) {}

Object::Object(std::uint32_t priority) : m_lastPriority(priority) {}

Object::~Object() noexcept {}

void Object::assign(Object&& other)
{
    this->m_serialized   = std::move(other.m_serialized);
    this->m_lastPriority = other.m_lastPriority;
    this->m_updateAge    = 0;
}

bool Object::tryUpdate(Object&& other)
{
    if (other.canUpdate(*this))
    {
        this->assign(std::move(other));
        return true;
    }
    return false;
}

bool Object::canUpdate(const Object& other) const
{
    return this->m_lastPriority >= other.m_lastPriority || other.m_updateAge >= OBJ_OUTDATED;
}

void Object::set_serialized(std::string&& serialized)
{
    m_serialized = std::move(serialized);
}

const std::string& Object::get_serialized() const
{
    return m_serialized;
}

Object& Object::operator++()
{
    ++m_updateAge;
    return *this;
}

}  // namespace object
