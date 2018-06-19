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

/// @def GPS_NR_SATS_GOOD
/// Good number of satellites
#define GPS_NR_SATS_GOOD 7

/// @def GPS_FIX_GOOD
/// Good fix quality
#define GPS_FIX_GOOD 1

/// @def GPS_HOR_DILUTION_GOOD
/// Good horizontal dilution
#define GPS_HOR_DILUTION_GOOD 1.0

using namespace object;

namespace data
{
GpsData::GpsData() : Data()
{}

GpsData::GpsData(const GpsPosition& crPosition, bool vGround)
    : Data(), mPosition(crPosition), mGroundMode(vGround)
{
    mProcessor.process(mPosition);
}

GpsData::~GpsData() noexcept
{}

std::string GpsData::getSerialized()
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    return (++mPosition).getSerialized();
}

bool GpsData::update(Object&& rvPosition)
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    if(mPositionLocked)
    {
        throw std::runtime_error("Position was locked before.");
    }
    bool updated = mPosition.tryUpdate(std::move(rvPosition));
    if(updated)
    {
        mProcessor.process(mPosition);
    }
    return (mPositionLocked = updated && mGroundMode && isPositionGood());
}

Position GpsData::getPosition()
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    return mPosition.getPosition();
}

bool GpsData::isPositionGood()
{
    return mPosition.getNrOfSatellites() >= GPS_NR_SATS_GOOD
           && mPosition.getFixQuality() >= GPS_FIX_GOOD
           && mPosition.getDilution() <= GPS_HOR_DILUTION_GOOD;
}

}  // namespace data
