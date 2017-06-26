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
        : Data<struct SensorInfo>()
{
    mPress.value = ICAO_STD_A;
}

SensorData::~SensorData() noexcept
{
}

void SensorData::update(const struct SensorInfo& cr_info, std::int32_t prio)
{
    if (!cr_info.mdaStr.empty())
    {
        setMdaStr(cr_info.mdaStr, prio);
    }
    if (!cr_info.mwvStr.empty())
    {
        setMwvStr(cr_info.mwvStr, prio);
    }
    if (cr_info.press != SI_PRESS_NA)
    {
        setPress(cr_info.press, prio);
    }
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

void SensorData::setMwvStr(const std::string& cr_mwv, std::int32_t prio)
{
    boost::lock_guard<boost::mutex> lock(mMwvData.mutex);
    mMwvData.trySetValue(cr_mwv, prio);
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

void SensorData::setMdaStr(const std::string& cr_mda, std::int32_t prio)
{
    boost::lock_guard<boost::mutex> lock(mMdaData.mutex);
    mMdaData.trySetValue(cr_mda, prio);
}

double SensorData::getPress()
{
    boost::lock_guard<boost::mutex> lock(mPress.mutex);
    return mPress.value;
}

void SensorData::init(const struct util::SensorInfo& cr_info)
{
    mMdaData.trySetValue(cr_info.mdaStr, 0);
    mMwvData.trySetValue(cr_info.mwvStr, 0);
    mPress.trySetValue(cr_info.press, 0);
}

void SensorData::setPress(double p, std::int32_t prio)
{
    boost::lock_guard<boost::mutex> lock(mPress.mutex);
    mPress.trySetValue(p, prio);
}

}  // namespace data
