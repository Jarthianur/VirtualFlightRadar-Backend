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

#include "Sensor.h"

namespace feed
{
namespace data
{
namespace object
{
Atmosphere::Atmosphere() : Object(0)
{}

Atmosphere::Atmosphere(std::uint32_t vPriority) : Object(vPriority)
{}

Atmosphere::Atmosphere(double vPress) : Object(0), mPressure(vPress)
{}

Atmosphere::~Atmosphere() noexcept
{}

void Atmosphere::assign(const Object& crOther)
{
    const Atmosphere& crUpdate = static_cast<const Atmosphere&>(crOther);
    this->mMdaStr              = crUpdate.mMdaStr;
    this->mPressure            = crUpdate.mPressure;
    this->mLastPriority        = crUpdate.mLastPriority;
}

const std::string& Atmosphere::getMdaStr() const
{
    return mMdaStr;
}

double Atmosphere::getPressure() const
{
    return mPressure;
}

void Atmosphere::setMdaStr(const std::string& crStr)
{
    mMdaStr = crStr;
}

void Atmosphere::setPressure(double vPress)
{
    mPressure = vPress;
}

Wind::Wind() : Object(0)
{}

Wind::Wind(std::uint32_t vPriority) : Object(vPriority)
{}

Wind::~Wind() noexcept
{}

void Wind::assign(const Object& crOther)
{
    const Wind& crUpdate = static_cast<const Wind&>(crOther);
    this->mMwvStr        = crUpdate.mMwvStr;
    this->mLastPriority  = crUpdate.mLastPriority;
}

const std::string& Wind::getMwvStr() const
{
    return mMwvStr;
}

void Wind::setMwvStr(const std::string& crStr)
{
    mMwvStr = crStr;
}
}
}
}
