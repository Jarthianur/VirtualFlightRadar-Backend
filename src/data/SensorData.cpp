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

using namespace util;

namespace data
{

SensorData::SensorData()
{
    mPress.value = ICAO_STD_A;
}

SensorData::~SensorData() noexcept
{
}

std::string SensorData::getMWVstr()
{
    boost::lock_guard<boost::mutex> lock(mMWV.mutex);
    if (mMWV.valueValid)
    {
        return mMWV.getValue() + "\r\n";
    } else
    {
        return "";
    }
}

void SensorData::setMWVstr(Priority prio, const std::string& cr_mwv)
{
    boost::lock_guard<boost::mutex> lock(mMWV.mutex);
    mMWV.update(cr_mwv, prio);
}

std::string SensorData::getMDAstr()
{
    boost::lock_guard<boost::mutex> lock(mMDA.mutex);
    if (mMDA.valueValid)
    {
        return mMWV.getValue() + "\r\n";
    } else
    {
        return "";
    }
}

void SensorData::setMDAstr(Priority prio, const std::string& cr_mda)
{
    boost::lock_guard<boost::mutex> lock(mMDA.mutex);
    mMDA.update(cr_mda, prio);
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

}  // namespace data
