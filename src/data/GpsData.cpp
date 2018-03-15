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

#include <algorithm>
#include <stdexcept>
#include <boost/thread/lock_guard.hpp>

/// Define GPS metrics
#define GPS_NR_SATS_GOOD 7
#define GPS_FIX_GOOD 1
#define GPS_HOR_DILUTION_GOOD 1.0

using namespace data::object;

namespace data
{
GpsData::GpsData() : Data()
{}

GpsData::GpsData(const GpsPosition& crPosition) : Data(), mPosition(crPosition)
{}

GpsData::~GpsData() noexcept
{}

std::string GpsData::getSerialized()
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    return mPosition.getSerialized();
}

bool GpsData::update(const Object& crPosition, std::size_t vSlot)
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    if(mPositionLocked)
    {
        throw std::runtime_error("Position was locked before.");
    }
    try
    {
        bool updated = mPosition.tryUpdate(crPosition, ++mAttempts.at(vSlot));
        if(updated)
        {
            std::fill(mAttempts.begin(), mAttempts.end(), 0);
            mPosition.setSerialized(mProcessor.process(mPosition));
        }
        return (mPositionLocked = updated && mPosition.ground
                                  && (mPosition.nrSats >= GPS_NR_SATS_GOOD
                                      && mPosition.fixQa >= GPS_FIX_GOOD
                                      && mPosition.dilution <= GPS_HOR_DILUTION_GOOD));
    }
    catch(const std::out_of_range&)
    {
        return false;
    }
}

std::size_t GpsData::registerSlot()
{
    mAttempts.push_back(0);
    return mAttempts.size() - 1;
}

Position GpsData::getGpsPosition()
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    return mPosition.position;
}

}  // namespace data
