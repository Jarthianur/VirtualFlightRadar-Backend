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

#include "AircraftData.h"

#include <algorithm>
#include <exception>
#include <iterator>
#include <utility>
#include <boost/thread/lock_guard.hpp>

#include "../util/Logger.h"
#include "../util/Position.h"
#include "Aircraft.h"

#ifndef ESTIMATED_TRAFFIC
#define ESTIMATED_TRAFFIC 1
#endif

using namespace util;
using namespace aircraft;

namespace data
{
AircraftData::AircraftData() : AircraftData(std::numeric_limits<std::int32_t>::max())
{}

AircraftData::AircraftData(std::int32_t vMaxDist) : mProcessor(vMaxDist)
{
    mContainer.reserve(ESTIMATED_TRAFFIC);
    mIndexMap.reserve(ESTIMATED_TRAFFIC * 2);
}

AircraftData::~AircraftData() noexcept
{}

void AircraftData::init(Aircraft vAircraft)
{
    std::uint64_t dummy = 0;
    update(vAircraft, 0, dummy);
}

std::vector<Aircraft>::iterator AircraftData::find(const std::string& crId)
{
    const auto it = mIndexMap.find(crId);
    if(it == mIndexMap.cend())
    {
        return mContainer.end();
    }
    else
    {
        return mContainer.begin() + static_cast<std::int64_t>(it->second);
    }
}

std::string AircraftData::processAircrafts(const struct util::GpsPosition& crBasePos,
                                           double vAtmPress)
{
    boost::lock_guard<boost::mutex> lock(this->mMutex);
    std::string dest_str;
    std::size_t index = 0;
    bool del          = false;
    auto it           = mContainer.begin();

    while(it != mContainer.end())
    {
        try
        {
            // if no FLARM msg received after x, assume target has Transponder
            if(++(it->getUpdateAge()) == AC_NO_FLARM_THRESHOLD)
            {
                it->setTargetType(Aircraft::TargetType::TRANSPONDER);
            }

            if(it->getUpdateAge() >= AC_DELETE_THRESHOLD)
            {
                del = true;
                mIndexMap.erase(it->getId());
                it = mContainer.erase(it);
            }
            else
            {
                if(it->getUpdateAge() < AC_OUTDATED)
                {
                    dest_str += mProcessor.process(*it, crBasePos, vAtmPress);
                }
                ++it;
                ++index;
            }
            if(del && it != mContainer.end())
            {
                mIndexMap.at(it->getId()) = index;
            }
        }
        catch(const std::exception& e)
        {
            Logger::warn("(AircraftData) processAircrafts: ", e.what());
        }
    }
    return dest_str;
}

void AircraftData::update(const Aircraft& rUpdate, std::uint32_t vPriority,
                          std::uint64_t&)
{
        auto known_ac = find(rUpdate.getId());
    if(known_ac != mContainer.end())
    {
        if(known_ac->getTargetType() == Aircraft::TargetType::TRANSPONDER
           || rUpdate.getTargetType() == Aircraft::TargetType::FLARM)
        {
            if(vPriority * ++(known_ac->getUpdateAttempts())
               >= known_ac->getLastPriority())
            {
                known_ac->update(rUpdate, vPriority);
            }
        }
    }
    else
    {
        mIndexMap.insert({rUpdate.getId(), mContainer.size()});
        mContainer.push_back(rUpdate);
    }
}

}  // namespace aircraft
