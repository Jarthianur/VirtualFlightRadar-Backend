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

#include "SensorData.h"

#include <boost/thread/lock_guard.hpp>

SensorData::SensorData()
{
    mPress.value = ICAO_STD_A;
}

SensorData::~SensorData() noexcept
{
}

std::string SensorData::getWVstr()
{
    boost::lock_guard<boost::mutex> lock(mWV.mutex);
    if (mWV.valid)
    {
        mWV.valid = false;
        return mWV.value + "\r\n";
    }
    else
    {
        return "";
    }
}

void SensorData::setWVstr(Priority prio, const std::string& wv)
{
    boost::lock_guard<boost::mutex> lock(mWV.mutex);
    mWV.update(wv, prio);
}

double SensorData::getPress()
{
    boost::lock_guard<boost::mutex> lock(mPress.mutex);
    return mPress.value;
}

void SensorData::setPress(Priority prio, double p)
{
    boost::lock_guard<boost::mutex> lock(mPress.mutex);
    mPress.update(p, prio);
}
