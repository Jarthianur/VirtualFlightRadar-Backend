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

#include <stdexcept>
#include <boost/thread/lock_guard.hpp>

/// Define GPS metrics
#define GPS_NR_SATS_GOOD 7
#define GPS_FIX_GOOD 1
#define GPS_HOR_DILUTION_GOOD 1.0

namespace feed
{
using namespace data::object;

namespace data
{
GpsData::GpsData() : Data()
{}

GpsData::GpsData(const ExtGpsPosition& crPosition) : Data(), mBasePos(crPosition)
{}

GpsData::~GpsData() noexcept
{}

std::string GpsData::getSerialized()
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    return mGpsModule.genGprmcStr(mBasePos) + mGpsModule.genGpggaStr(mBasePos);
}

bool GpsData::update(const Object& crPosition, std::size_t vSlot)
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    if(mPosLocked)
    {
        throw std::runtime_error("Position was locked before.");
    }
    try
    {
        bool updated = mBasePos.tryUpdate(crPosition, ++mFeedAttempts.at(vSlot));
        if(updated)
        {
            clearAttempts(mFeedAttempts);
        }
        return (mPosLocked = updated && mBasePos.ground
                             && (mBasePos.nrSats >= GPS_NR_SATS_GOOD
                                 && mBasePos.fixQa >= GPS_FIX_GOOD
                                 && mBasePos.dilution <= GPS_HOR_DILUTION_GOOD));
    }
    catch(const std::out_of_range&)
    {
        return false;
    }
}

GpsPosition GpsData::getGpsPosition()
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    return mBasePos.position;
}
}
}  // namespace data
