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

#include "WindData.h"

#include <algorithm>
#include <stdexcept>
#include <boost/thread/lock_guard.hpp>

using namespace data::object;

namespace data
{
WindData::WindData() : Data()
{}

WindData::WindData(const object::Wind& crWind) : Data(), mWind(crWind)
{}

WindData::~WindData() noexcept
{}

std::string WindData::getSerialized()
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    std::string tmp(mWind.getSerialized());
    mWind.setSerialized("");
    return tmp;
}

bool WindData::update(const Object& crWind, std::size_t vSlot)
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    try
    {
        bool updated = mWind.tryUpdate(crWind, ++mAttempts.at(vSlot));
        if(updated)
        {
            std::fill(mAttempts.begin(), mAttempts.end(), 0);
        }
        return updated;
    }
    catch(const std::out_of_range&)
    {
        return false;
    }
}

std::size_t WindData::registerSlot()
{
    mAttempts.push_back(0);
    return mAttempts.size() - 1;
}

}  // namespace data
