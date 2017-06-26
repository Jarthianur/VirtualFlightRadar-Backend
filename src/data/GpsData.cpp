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

void GpsData::init(const struct util::ExtGpsPosition& cr_pos)
{
    mBasePos.trySetValue(cr_pos, 0);
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
    return mBasePos.value.position.altitude;
}

double GpsData::getBaseLat()
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    return mBasePos.value.position.latitude;
}

void GpsData::update(const struct util::ExtGpsPosition& cr_pos, std::int32_t prio)
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    mBasePos.trySetValue(cr_pos, prio);
}

double GpsData::getBaseLong()
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    return mBasePos.value.position.longitude;
}

struct util::ExtGpsPosition GpsData::getBasePos()
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    return mBasePos.value;
}

} // namespace data
