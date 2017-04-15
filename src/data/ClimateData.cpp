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

#include "ClimateData.h"
#include <boost/thread/lock_guard.hpp>

ClimateData::ClimateData()
        : mPress(ICAO_STD_A),
          mTemp(ICAO_STD_T)
{
}

ClimateData::~ClimateData() noexcept
{
}

std::string ClimateData::extractWV()
{
    boost::lock_guard<boost::mutex> lock(this->mMutex);
    mWVvalid = false;
    return mWV + "\r\n";
}

bool ClimateData::isValid()
{
    boost::lock_guard<boost::mutex> lock(this->mMutex);
    return mWVvalid;
}

void ClimateData::insertWV(const std::string& wv)
{
    boost::lock_guard<boost::mutex> lock(this->mMutex);
    mWV = wv;
    mWVvalid = true;
}

/**
 * Note:
 * Assuming that climate input is received every (few) second(s),
 * there is no need to enforce threadsafety and reduce performance by locks.
 * These single operations are atomic, hence no undefined value may occure.
 * Worst case is, that the reader gets an old value which is acceptable with
 * this frequency of updates.
 */

double ClimateData::getPress()
{
    //boost::lock_guard<boost::mutex> lock(this->mMutex);
    return mPress;
}

void ClimateData::setPress(double p)
{
    //boost::lock_guard<boost::mutex> lock(this->mMutex);
    mPress = p;
}

void ClimateData::setTemp(double t)
{
    //boost::lock_guard<boost::mutex> lock(this->mMutex);
    mTemp = t;
}

double ClimateData::getTemp()
{
    //boost::lock_guard<boost::mutex> lock(this->mMutex);
    return mTemp;
}
