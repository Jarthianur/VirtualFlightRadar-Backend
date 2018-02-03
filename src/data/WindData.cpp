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

#include <boost/thread/lock_guard.hpp>
#include "WindData.h"

using namespace util;

namespace data
{

WindData::WindData()
        : Data<struct Wind>()
{
}

WindData::~WindData() noexcept
{
}

void WindData::update(const struct Wind& crWind, std::uint32_t vPriority,
                      std::uint32_t& rAttempts)
{
    boost::lock_guard<boost::mutex> lock(mWind.mutex);
    if (mWind.trySetValue(crWind, vPriority, rAttempts))
    {
        rAttempts = 0;
    }
}

std::string WindData::getMwvStr()
{
    boost::lock_guard<boost::mutex> lock(mWind.mutex);
    if (mWind.isValueValid)
    {
        return mWind.getValue().mwvStr + "\n";
    }
    else
    {
        return "";
    }
}

void WindData::init(struct Wind vWind)
{
    std::uint32_t dummy = 0;
    mWind.trySetValue(vWind, 0, dummy);
}

}  // namespace data
