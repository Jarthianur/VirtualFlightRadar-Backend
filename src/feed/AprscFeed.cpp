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

#include "AprscFeed.h"

#include <stdexcept>

#include "../Logger.hpp"
#include "../config/Configuration.h"
#include "../data/AircraftData.h"
#include "../object/Aircraft.h"
#include "parser/AprsParser.h"

#ifdef COMPONENT
#undef COMPONENT
#endif
#define COMPONENT "(AprscFeed)"

namespace feed
{
parser::AprsParser AprscFeed::smParser;

AprscFeed::AprscFeed(const std::string& crName, const config::KeyValueMap& crKvMap,
                     std::shared_ptr<data::AircraftData> pData, std::int32_t vMaxHeight)
    : Feed(crName, crKvMap, pData)
{
    smParser.setMaxHeight(vMaxHeight);
    mLoginStrIt = mKvMap.find(KV_KEY_LOGIN);
    if(mLoginStrIt == mKvMap.end())
    {
        logger.warn(COMPONENT " could not find: ", mName, "." KV_KEY_LOGIN);
        throw std::logic_error("No login given");
    }
}

AprscFeed::~AprscFeed() noexcept
{}

Feed::Protocol AprscFeed::getProtocol() const
{
    return Protocol::APRS;
}

bool AprscFeed::process(const std::string& crResponse) noexcept
{
    object::Aircraft ac(getPriority());
    if(smParser.unpack(crResponse, ac))
    {
        mpData->update(std::move(ac));
    }
    return true;
}

const std::string& AprscFeed::getLoginStr() const
{
    return mLoginStrIt->second;
}

}  // namespace feed
