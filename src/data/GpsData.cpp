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

#include "GpsData.h"

#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>

namespace data
{

GpsData::GpsData()
{
}

GpsData::~GpsData() noexcept
{
}

void GpsData::init(struct util::ExtGpsPosition pos)
{
    std::uint32_t dummy = 0;
    mBasePos.trySetValue(pos, 0, dummy);
}

void GpsData::update(const struct util::ExtGpsPosition& cr_pos, std::uint32_t prio,
                     std::uint32_t& rAttempts)
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    if (mBasePos.trySetValue(cr_pos, prio, rAttempts))
    {
        rAttempts = 0;
    }
}

std::string GpsData::getGpsStr()
{
    std::string gps = mGpsMod.genGprmcStr(getBasePos());
    gps.append(mGpsMod.genGpggaStr(getBasePos()));
    return gps;
}

std::int32_t GpsData::getBaseAlt()
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    return mBasePos.getValue().position.altitude;
}

double GpsData::getBaseLat()
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    return mBasePos.getValue().position.latitude;
}

double GpsData::getBaseLong()
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    return mBasePos.getValue().position.longitude;
}

struct util::ExtGpsPosition GpsData::getBasePos()
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    return mBasePos.getValue();
}

} // namespace data
