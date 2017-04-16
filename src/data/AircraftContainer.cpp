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

void AircraftContainer::initProcessor(double proc_lat, double proc_lon,
                                      std::int32_t proc_alt)
{
    mAcProc.init(proc_lat, proc_lon, proc_alt);
}

ssize_t AircraftContainer::find(const std::string& id)
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
            it->incValid();
            // if no FLARM msg received after x, assume target has Transponder
            if (it->getValid() == AC_NO_FLARM_THRESHOLD)
            {
                it->setTargetT(Aircraft::TargetType::TRANSPONDER);
            }

            if (it->getValid() >= AC_DELETE_THRESHOLD)
            {
                del = true;
                mIndexMap.erase(mIndexMap.find(it->getID()));
                mCont.erase(it);
            }
            else
            {
                if (it->getValid() < AC_INVALIDATE)
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

void AircraftContainer::insertAircraft(const Aircraft& update)
{
    boost::lock_guard<boost::mutex> lock(this->mMutex);
    ssize_t i;

    if ((i = find(update.getID())) == AC_NOT_FOUND)
    {
        mCont.push_back(update);
        mIndexMap.insert( { update.getID(), mCont.size() - 1 });
    }
    else
    {
        Aircraft& known_ac = mCont.at(i);
        if (known_ac.getTargetT() == Aircraft::TargetType::TRANSPONDER || update.getTargetT()
                == Aircraft::TargetType::FLARM)
        {
            known_ac.update(update);
        }
    }
}
