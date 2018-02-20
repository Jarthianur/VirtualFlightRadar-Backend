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

#include "AtmosphereData.h"
#include <boost/thread/lock_guard.hpp>

using namespace util;

namespace data
{
AtmosphereData::AtmosphereData() : Data<struct Atmosphere>()
{}

AtmosphereData::~AtmosphereData() noexcept
{}

void AtmosphereData::update(const struct Atmosphere& crAtmos, std::uint32_t vPriority,
                            std::uint64_t& rAttempts)
{
    if(mAtmosphere.trySetValue(crAtmos, vPriority, rAttempts))
    {
        rAttempts = 0;
    }
}

std::string AtmosphereData::getMdaStr()
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    return mAtmosphere.getValue().mdaStr + "\n";
}

double AtmosphereData::getAtmPress()
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    return mAtmosphere.getValue().pressure;
}

void AtmosphereData::init(struct util::Atmosphere vAtmos)
{
    std::uint64_t dummy = 0;
    mAtmosphere.trySetValue(vAtmos, 0, dummy);
}

}  // namespace data
