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

#include "SbsFeed.h"

#include <unordered_map>

#include "../config/Configuration.h"
#include "../data/AircraftData.h"
#include "../object/Aircraft.h"
#include "client/Client.h"
#include "client/ClientManager.h"
#include "parser/SbsParser.h"

namespace feed
{
parser::SbsParser SbsFeed::smParser;

SbsFeed::SbsFeed(const std::string& crName, const config::KeyValueMap& crKvMap,
                 std::shared_ptr<data::AircraftData> pData, std::int32_t vMaxHeight)
    : Feed(crName, crKvMap, pData)
{
    smParser.setMaxHeight(vMaxHeight);
}

SbsFeed::~SbsFeed() noexcept
{}

void SbsFeed::registerToClient(client::ClientManager& rManager)
{
    mSubsribedClient = rManager.subscribe(
        shared_from_this(), {mKvMap.find(KV_KEY_HOST)->second, mKvMap.find(KV_KEY_PORT)->second},
        client::ClientManager::Protocol::SBS);
}

void SbsFeed::process(const std::string& crResponse) noexcept
{
    object::Aircraft ac(getPriority());
    if(smParser.unpack(crResponse, ac))
    {
        mpData->update(std::move(ac));
    }
}

}  // namespace feed
