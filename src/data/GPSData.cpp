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

GPSData::GPSData()
{
}

GPSData::~GPSData() noexcept
{
}

void GPSData::setDefaults(double b_lat, double b_lon, std::int32_t b_alt, double geoid)
{
    setGGAstr(Priority::DONTCARE, mGPSfix.ggafix(b_lat, b_lon, b_alt, geoid));
    setRMCstr(Priority::DONTCARE, mGPSfix.rmcfix(b_lat, b_lon, b_alt, geoid));
    struct GPSPosition base;
    base.latitude = b_lat;
    base.longitude = b_lon;
    base.altitude = b_alt;
    base.nrSats = 5;
    base.fixQa = 1;
    setBasePos(Priority::DONTCARE, base);
}

void GPSData::setGGAstr(Priority prio, const std::string& gga)
{
    boost::lock_guard<boost::mutex> lock(mGGAstr.mutex);
    mGGAstr.update(gga, prio);
}

std::string GPSData::getGGAstr()
{
    boost::lock_guard<boost::mutex> lock(mGGAstr.mutex);
    return mGGAstr.value;
}

void GPSData::setRMCstr(Priority prio, const std::string& rmc)
{
    boost::lock_guard<boost::mutex> lock(mRMCstr.mutex);
    mRMCstr.update(rmc, prio);
}

std::string GPSData::getGPSstr()
{
    std::string gps = getRMCstr();
    gps.append("\r\n");
    gps.append(getGGAstr());
    gps.append("\r\n");
    return gps;
}

std::string GPSData::getRMCstr()
{
    boost::lock_guard<boost::mutex> lock(mRMCstr.mutex);
    return mRMCstr.value;
}

std::int32_t GPSData::getBaseAlt()
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    return mBasePos.value.altitude;
}

double GPSData::getBaseLat()
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    return mBasePos.value.latitude;
}

void GPSData::setBasePos(Priority prio, const struct GPSPosition& pos)
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    mBasePos.update(pos, prio);
}

double GPSData::getBaseLong()
{
    boost::lock_guard<boost::mutex> lock(mBasePos.mutex);
    return mBasePos.value.longitude;
}
