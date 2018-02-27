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

namespace data
{
namespace object
{
Atmosphere::Atmosphere() : Object(0)
{}

Atmosphere::Atmosphere(std::uint32_t vPriority) : Object(vPriority)
{}

Atmosphere::~Atmosphere() noexcept
{}

Object& Atmosphere::operator=(const boost::tuple<const Object&, std::uint32_t>& crOther)
{
    const Atmosphere& crUpdate = static_cast<const Atmosphere&>(crOther.get<0>());
    this->mMdaStr              = crUpdate.mMdaStr;
    this->mPressure            = crUpdate.mPressure;
    this->mLastPriority        = crOther.get<1>();
    return *this;
}

const std::string& Atmosphere::getMdaStr() const
{
    return mMdaStr;
}

double Atmosphere::getPressure() const
{
    return mPressure;
}

Wind::Wind() : Object(0)
{}

Wind::Wind(std::uint32_t vPriority) : Object(vPriority)
{}

Wind::~Wind() noexcept
{}

Object& Wind::operator=(const boost::tuple<const Object&, std::uint32_t>& crOther)
{
    const Wind& crUpdate = static_cast<const Wind&>(crOther.get<0>());
    this->mMwvStr        = crUpdate.mMwvStr;
    this->mLastPriority  = crOther.get<1>();
    return *this;
}

const std::string& Wind::getMwvStr() const
{
    return mMwvStr;
}
}
}
