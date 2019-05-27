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

#include "data/AircraftData.h"

#include <iterator>
#include <stdexcept>

using namespace object;

namespace data
{
AircraftData::AircraftData() : AircraftData(0) {}

AircraftData::AircraftData(std::int32_t maxDist) : Data(), m_processor(maxDist) {}

bool AircraftData::update(Object&& aircraft)
{
    Aircraft&& update = static_cast<Aircraft&&>(aircraft);
    auto       result = m_container.insert(std::move(update));
    if (!result.second)
    {
        result.first->aircraft.tryUpdate(std::move(update));
    }
    return true;
}

void AircraftData::set_environment(const Position& position, double atmPress)
{
    m_processor.referTo(position, atmPress);
}

void AircraftData::access(const accessor_fn& func)
{
    Container::Iterator iter = m_container.begin();
    while (iter != m_container.end())
    {
        ++(iter->aircraft);
        try
        {
            if (iter->aircraft.get_updateAge() == NO_FLARM_THRESHOLD)
            {
                iter->aircraft.set_targetType(Aircraft::TargetType::TRANSPONDER);
            }
            if (iter->aircraft.get_updateAge() >= DELETE_THRESHOLD)
            {
                Container::KeyType key = iter.getKey();
                ++iter;
                m_container.erase(key);
            }
            else
            {
                m_processor.process(iter->aircraft);
                func(iter->aircraft);
                ++iter;
            }
        }
        catch (const std::exception&)
        {}
    }
}

}  // namespace data
