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

#include "AircraftContainer.h"

#include <algorithm>
#include <exception>
#include <iterator>
#include <utility>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/pthread/mutex.hpp>

#include "../util/Logger.h"
#include "../util/Position.h"
#include "Aircraft.hpp"

using namespace util;

namespace aircraft
{
AircraftContainer::AircraftContainer()
{
    mCont.reserve(20);
    mIndexMap.reserve(40);
}

AircraftContainer::~AircraftContainer() noexcept
{}

std::vector<Aircraft>::iterator AircraftContainer::find(const std::string& cr_id)
{
    const auto it = mIndexMap.find(cr_id);
    if(it == mIndexMap.cend())
    {
        return mCont.end();
    }
    else
    {
        return mCont.begin() + it->second;
    }
}

std::string AircraftContainer::processAircrafts(const struct util::GpsPosition& crBasePos,
                                                double vAtmPress)
{
    boost::lock_guard<boost::mutex> lock(this->mMutex);
    std::string dest_str;
    std::size_t index = 0;
    bool del          = false;
    auto it           = mCont.begin();

    while(it != mCont.end())
    {
        try
        {
            // if no FLARM msg received after x, assume target has Transponder
            if(++(it->getUpdateAge()) == AC_NO_FLARM_THRESHOLD)
            {
                it->setTargetT(Aircraft::TargetType::TRANSPONDER);
            }

            if(it->getUpdateAge() >= AC_DELETE_THRESHOLD)
            {
                del = true;
                mIndexMap.erase(it->getId());
                it = mCont.erase(it);
            }
            else
            {
                if(it->getUpdateAge() < AC_INVALIDATE)
                {
                    dest_str += mAcProc.process(*it, crBasePos, vAtmPress);
                }
                ++it;
                ++index;
            }
            if(del && it != mCont.end())
            {
                mIndexMap.at(it->getId()) = index;
            }
        }
        catch(const std::exception& e)
        {
            Logger::warn("(AircraftContainer) processAircrafts: ", e.what());
        }
    }
    return dest_str;
}

void AircraftContainer::upsert(Aircraft& cr_update, std::uint32_t prio)
{
    boost::lock_guard<boost::mutex> lock(this->mMutex);
    auto known_ac = find(cr_update.getId());
    if(known_ac != mCont.end())
    {
        if(known_ac->getTargetT() == Aircraft::TargetType::TRANSPONDER
           || cr_update.getTargetType() == Aircraft::TargetType::FLARM)
        {
            if(prio * ++(known_ac->getUpdateAttempts()) >= known_ac->getLastPriority())
            {
                known_ac->update(cr_update, prio);
            }
        }
    }
    else
    {
        cr_update.setLastPriority(prio);
        mIndexMap.insert({cr_update.getId(), mCont.size()});
        mCont.push_back(cr_update);
    }
}

}  // namespace aircraft
