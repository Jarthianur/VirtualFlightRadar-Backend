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

namespace data
{

SensorData::SensorData()
{
    mPress.value = ICAO_STD_A;
}

SensorData::~SensorData() noexcept
{
}

std::string SensorData::getMwvStr()
{
    boost::lock_guard<boost::mutex> lock(mMwvData.mutex);
    if (mMwvData.valueValid)
    {
        return mMwvData.getValue() + "\n";
    } else
    {
        return "";
    }
}

void SensorData::setMwvStr(std::int32_t prio, const std::string& cr_mwv)
{
    boost::lock_guard<boost::mutex> lock(mMwvData.mutex);
    mMwvData.update(cr_mwv, prio);
}

std::string SensorData::getMdaStr()
{
    boost::lock_guard<boost::mutex> lock(mMdaData.mutex);
    if (mMdaData.valueValid)
    {
        return mMdaData.getValue() + "\n";
    } else
    {
        return "";
    }
}

void SensorData::setMdaStr(std::int32_t prio, const std::string& cr_mda)
{
    boost::lock_guard<boost::mutex> lock(mMdaData.mutex);
    mMdaData.update(cr_mda, prio);
}

double SensorData::getPress()
{
    boost::lock_guard<boost::mutex> lock(mPress.mutex);
    return mPress.value;
}

void SensorData::setPress(std::int32_t prio, double p)
{
    boost::lock_guard<boost::mutex> lock(mPress.mutex);
    mPress.update(p, prio);
}

}  // namespace data
