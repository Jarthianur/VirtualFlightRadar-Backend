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

#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "../aircraft/Aircraft.hpp"
#include "../aircraft/AircraftContainer.h"
#include "../config/Configuration.h"
#include "../network/client/AprscClient.h"
#include "../util/Logger.h"
#include "../VFRB.h"

using namespace util;

namespace feed
{

AprscFeed::AprscFeed(const std::string& cr_name, std::uint32_t prio,
        const config::keyValueMap& cr_kvmap)
        : Feed(cr_name, prio, cr_kvmap)
{
    auto it = mKvMap.find(KV_KEY_LOGIN);
    if (it == mKvMap.end())
    {
        Logger::warn("(AprscFeed) could not find: ", mName + "." KV_KEY_LOGIN);
        throw std::logic_error("No login given");
    } else
    {
        mpClient = std::unique_ptr<network::client::Client>(
                new network::client::AprscClient(mKvMap.find(KV_KEY_HOST)->second,
                        mKvMap.find(KV_KEY_PORT)->second, it->second, *this));
    }
}

AprscFeed::~AprscFeed() noexcept
{
}

void AprscFeed::process(const std::string& cr_res) noexcept
{
    aircraft::Aircraft ac;
    if (mParser.unpack(cr_res, ac))
    {
        VFRB::msAcCont.upsert(ac, mPriority);
    }
}

} // namespace feed
