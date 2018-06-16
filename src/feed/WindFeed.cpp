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

#include "WindFeed.h"

#include <unordered_map>

#include "../config/Configuration.h"
#include "../data/WindData.h"
#include "../object/Wind.h"

namespace feed
{
WindFeed::WindFeed(const std::string& crName, const config::KeyValueMap& crKvMap,
                   std::shared_ptr<data::WindData>& pData)
    : Feed(crName, crKvMap), mpData(pData)

{
    mDataSlot = mpData->registerSlot();
}

WindFeed::~WindFeed() noexcept
{}

void WindFeed::process(const std::string& crResponse) noexcept
{
    object::Wind wind(getPriority());
    if(mParser.unpack(crResponse, wind))
    {
        mpData->update(std::move(wind), mDataSlot);
    }
}

}  // namespace feed
