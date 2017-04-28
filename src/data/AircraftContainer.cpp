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

#include <boost/thread/lock_guard.hpp>
#include <boost/thread/pthread/mutex.hpp>
#include <algorithm>
#include <exception>
#include <iterator>
#include <utility>

#include "../util/Logger.h"
#include <iostream>

#define AC_INVALIDATE         (4)
#define AC_DELETE_THRESHOLD   (120)
#define AC_NO_FLARM_THRESHOLD (AC_INVALIDATE)

AircraftContainer::AircraftContainer()
        : mAcProc(),
          mCont()
{
    mCont.reserve(20);
    mIndexMap.reserve(40);
}

AircraftContainer::~AircraftContainer() noexcept
{
}

std::vector<Aircraft>::iterator AircraftContainer::find(const std::string& id)
{
    const auto it = mIndexMap.find(id);
    if (it == mIndexMap.cend())
    {
        return mCont.end();
    }
    else
    {
        return mCont.begin() + it->second;
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
            it->incUpdateAge();
            // if no FLARM msg received after x, assume target has Transponder
            if (it->getUpdateAge() == AC_NO_FLARM_THRESHOLD)
            {
                it->setTargetT(Aircraft::TargetType::TRANSPONDER);
            }

            if (it->getUpdateAge() >= 4)
            {
                del = true;
                mIndexMap.erase(it->getID());
                it = mCont.erase(it);
            }
            else
            {
                if (it->getUpdateAge() < AC_INVALIDATE)
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
        catch (const std::exception& e)
        {
            Logger::warn("(AircraftContainer) processAircrafts: ", e.what());
        }
    }
    return dest_str;
}

void AircraftContainer::insertAircraft(const Aircraft& update, Priority prio)
{
    boost::lock_guard<boost::mutex> lock(this->mMutex);
    auto known_ac = find(update.getID());
    if (known_ac != mCont.end())
    {
        if (known_ac->getTargetT() == Aircraft::TargetType::TRANSPONDER || update.getTargetT()
                == Aircraft::TargetType::FLARM)
        {
            bool write = known_ac->getAttemptValid();
            if (!write)
            {
                if (prio > known_ac->getLastPriority() || (prio
                        == known_ac->getLastPriority()
                                                           && prio != Priority::LESSER))
                {
                    write = true;
                }
            }
            if (write)
            {
                known_ac->update(update, prio);
            }
            else
            {
                known_ac->setAttemptValid();
            }
        }
    }
    else
    {
        mIndexMap.insert( { update.getID(), mCont.size() });
        mCont.push_back(update);
    }
}
