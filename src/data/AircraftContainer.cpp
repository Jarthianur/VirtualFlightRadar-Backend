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

#include "AircraftContainer.h"

#include <cstdint>
#include <exception>
#include <iterator>
#include <boost/thread/lock_guard.hpp>

#include "../util/Logger.h"

AircraftContainer::AircraftContainer()
        : mAcProc(),
          mCont()
{
    mCont.reserve(20);
}

AircraftContainer::~AircraftContainer() noexcept
{
}

void AircraftContainer::initProcessor(double proc_lat, double proc_lon, std::int32_t proc_alt)
{
    mAcProc.init(proc_lat, proc_lon, proc_alt);
}

ssize_t AircraftContainer::find(std::string& id)
{
    const auto it = mIndexMap.find(id);
    if (it == mIndexMap.cend())
    {
        return AC_NOT_FOUND;
    }
    else
    {
        return it->second;
    }
}

std::string AircraftContainer::processAircrafts()
{
    boost::lock_guard<boost::mutex> lock(this->mMutex);
    std::string dest_str;
    std::size_t index = 0;
    bool del = false;
    auto it = mCont.begin();

    while (it != mCont.end())
    {
        try
        {
            // if no FLARM msg received after x, assume target has Transponder
            if (++(it->mValid) >= AC_NO_FLARM_THRESHOLD)
            {
                it->mTargetType = Aircraft::TargetType::TRANSPONDER;
            }
            if (it->mValid >= AC_DELETE_THRESHOLD)
            {
                del = true;
                mIndexMap.erase(mIndexMap.find(it->getID()));
                mCont.erase(it);
            }
            else
            {
                if (it->mValid < AC_INVALIDATE)
                {
                    dest_str += mAcProc.process(*it);
                }
                ++it;
                ++index;
            }
            if (del && it != mCont.end())
            {
                mIndexMap.at(it->getID()) = index;
            }
        }
        catch (std::exception& e)
        {
            Logger::warn("(AircraftContainer) processAircrafts: ", e.what());
        }
    }
    return dest_str;
}

void AircraftContainer::insertAircraft(std::string& id, double lat, double lon,
                                       std::int32_t alt)
{
    boost::lock_guard<boost::mutex> lock(this->mMutex);
    ssize_t i;
    if ((i = find(id)) == AC_NOT_FOUND)
    {
        Aircraft ac(id, lat, lon, alt);
        ac.mAltAsQNE = true;
        ac.mTargetType = Aircraft::TargetType::TRANSPONDER;
        mCont.push_back(ac);
        mIndexMap.insert( { id, mCont.size() - 1 });
    }
    else
    {
        Aircraft& ac = mCont.at(i);
        if (ac.mTargetType == Aircraft::TargetType::TRANSPONDER)
        {
            ac.mValid = 0;
            ac.mLatitude = lat;
            ac.mLongitude = lon;
            ac.mAltitude = alt;
            ac.mAltAsQNE = true;
            ac.mFullInfo = false;
        }
    }
}

void AircraftContainer::insertAircraft(std::string& id, double lat, double lon,
                                       std::int32_t alt, double gnd_spd, std::uint32_t id_t,
                                       std::int32_t ac_t, double climb_r, double turn_r,
                                       double heading)
{
    boost::lock_guard<boost::mutex> lock(this->mMutex);
    ssize_t i;
    if ((i = find(id)) == -1)
    {
        Aircraft ac(id, lat, lon, alt, gnd_spd, id_t, ac_t, climb_r, turn_r, heading);
        if (heading == A_VALUE_NA || gnd_spd == A_VALUE_NA || climb_r == A_VALUE_NA)
        {
            ac.mFullInfo = false;
        }
        else
        {
            ac.mFullInfo = true;
        }
        mCont.push_back(ac);
        mIndexMap.insert( { id, mCont.size() - 1 });
    }
    else
    {
        Aircraft& ac = mCont.at(i);
        if (heading == A_VALUE_NA || gnd_spd == A_VALUE_NA || climb_r == A_VALUE_NA)
        {
            ac.mFullInfo = false;
        }
        else
        {
            ac.mFullInfo = true;
        }
        ac.mAircraftType = ac_t;
        ac.mGndSpeed = gnd_spd;
        ac.mIDtype = id_t;
        ac.mValid = 0;
        ac.mLatitude = lat;
        ac.mLongitude = lon;
        ac.mAltitude = alt;
        ac.mClimbRate = climb_r;
        ac.mTurnRate = turn_r;
        ac.mHeading = heading;
        ac.mAltAsQNE = false;
        ac.mTargetType = Aircraft::TargetType::FLARM;
    }
}
