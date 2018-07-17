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

#include "AtmosphereFeed.h"

#include <unordered_map>

#include "../config/Configuration.h"
#include "../data/AtmosphereData.h"
#include "../object/Atmosphere.h"
#include "client/Client.hpp"
#include "client/ClientManager.h"
#include "parser/AtmosphereParser.h"

namespace feed
{
parser::AtmosphereParser AtmosphereFeed::smParser;

AtmosphereFeed::AtmosphereFeed(const std::string& crName, const config::KeyValueMap& crKvMap,
                               std::shared_ptr<data::AtmosphereData> pData)
    : Feed(crName, crKvMap, pData)
{}

AtmosphereFeed::~AtmosphereFeed() noexcept
{}

void AtmosphereFeed::registerToClient(client::ClientManager& rManager)
{
    mSubsribedClient = rManager.subscribe(
        shared_from_this(), {mKvMap.find(KV_KEY_HOST)->second, mKvMap.find(KV_KEY_PORT)->second},
        client::ClientManager::Protocol::SENSOR);
}

void AtmosphereFeed::process(const std::string& crResponse) noexcept
{
    object::Atmosphere atmos(getPriority());
    if(smParser.unpack(crResponse, atmos))
    {
        mpData->update(std::move(atmos));
    }
}

}  // namespace feed
