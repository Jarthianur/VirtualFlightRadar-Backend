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

#include "GpsFeed.h"

#include <stdexcept>
#include <unordered_map>

#include "../Logger.hpp"
#include "../config/Configuration.h"
#include "../data/GpsData.h"
#include "../object/GpsPosition.h"
#include "client/GpsdClient.h"

#include "../Logger.hpp"

#ifdef COMPONENT
#undef COMPONENT
#endif
#define COMPONENT "(GpsFeed)"

namespace feed
{
parser::GpsParser GpsFeed::smParser;

GpsFeed::GpsFeed(const std::string& crName, const config::KeyValueMap& crKvMap,
                 std::shared_ptr<data::GpsData> pData)
    : Feed(crName, crKvMap, pData)
{
    Logger::debug(crName, " constructed ", COMPONENT);
}

GpsFeed::~GpsFeed() noexcept
{
    Logger::debug(mName, " destructed ", COMPONENT);
}

void GpsFeed::registerClient(client::ClientManager& rManager)
{
    mSubsribedClient = rManager.subscribe(
        shared_from_this(), {mKvMap.find(KV_KEY_HOST)->second, mKvMap.find(KV_KEY_PORT)->second},
        client::ClientManager::Protocol::GPS);
}

void GpsFeed::process(const std::string& crResponse) noexcept
{
    Logger::debug(mName, " process called");
    object::GpsPosition pos(getPriority());
    if(smParser.unpack(crResponse, pos))
    {
        try
        {
            if(mpData->update(std::move(pos), mDataSlot))
            {
                throw std::runtime_error("received good position -> stop");
            }
        }
        catch(const std::runtime_error& e)
        {
            Logger::info(COMPONENT " ", mName, ": ", e.what());
            auto client = std::shared_ptr<client::Client>(mSubsribedClient);
            client->stop();
            Logger::debug("returned from gpsfeed stop call");
            return;
        }
    }
}

}  // namespace feed
