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

using namespace object;

namespace data
{
AtmosphereData::AtmosphereData() : Data()
{}

AtmosphereData::AtmosphereData(const Atmosphere& crAtmosphere) : Data(), mAtmosphere(crAtmosphere)
{}

AtmosphereData::~AtmosphereData() noexcept
{}

std::string AtmosphereData::getSerialized()
{
    std::lock_guard<std::mutex> lock(mMutex);
    return (++mAtmosphere).get_serialized();
}

bool AtmosphereData::update(Object&& rvAtmosphere)
{
    std::lock_guard<std::mutex> lock(mMutex);
    return mAtmosphere.tryUpdate(std::move(rvAtmosphere));
}

double AtmosphereData::getAtmPressure()
{
    std::lock_guard<std::mutex> lock(mMutex);
    return mAtmosphere.get_pressure();
}

}  // namespace data
