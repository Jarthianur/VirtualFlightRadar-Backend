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

namespace feed
{
namespace data
{
namespace object
{
Object::Object() : mLastPriority(0)
{}

Object::Object(std::uint32_t vPriority) : mLastPriority(vPriority)
{}

Object::~Object() noexcept
{}

bool Object::tryUpdate(const Object& crOther, std::uint64_t vAttempts)
{
    if(crOther.canUpdate(*this, vAttempts))
    {
        this->assign(crOther);
        return true;
    }
    return false;
}

bool Object::canUpdate(const Object& crOther, std::uint64_t vAttempts) const
{
    return this->mLastPriority * vAttempts >= crOther.mLastPriority;
}

void Object::setLastPriority(std::uint32_t vPriority)
{
    mLastPriority = vPriority;
}

std::uint32_t Object::getLastPriority() const
{
    return mLastPriority;
}
}
}
}
