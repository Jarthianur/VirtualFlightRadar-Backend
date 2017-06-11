/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

#include "GPSData.h"

#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>

namespace data
{

GPSData::GPSData()
{
}

GPSData::~GPSData() noexcept
{
}

void GPSData::setDefaults(double b_lat, double b_lon, std::int32_t b_alt,
                          double geoid)
{
    struct util::ExtGPSPosition base;
    base.position.latitude = b_lat;
    base.position.longitude = b_lon;
    base.position.altitude = b_alt;
    base.nrSats = 5;
    base.fixQa = 1;
    base.geoid = geoid;
    setBasePos(0, base);
}

std::string GPSData::getGPSstr()
{
    std::string gps = mGPSfix.rmcfix(getBasePos());
    gps.append(mGPSfix.ggafix(getBasePos()));
    return gps;
}

std::int32_t GPSData::getBaseAlt()
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    return mBasePos.value.position.altitude;
}

double GPSData::getBaseLat()
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    return mBasePos.value.position.latitude;
}

void GPSData::setBasePos(std::int32_t prio,
                         const struct util::ExtGPSPosition& cr_pos)
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    mBasePos.update(cr_pos, prio);
}

double GPSData::getBaseLong()
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    return mBasePos.value.position.longitude;
}

struct util::ExtGPSPosition GPSData::getBasePos()
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    return mBasePos.value;
}

} // namespace data
