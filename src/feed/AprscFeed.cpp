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

#include "../config/Configuration.h"
#include "../parser/AprsParser.h"
#include "../tcp/client/AprscClient.h"
#include "../util/Logger.h"

using namespace util;

namespace feed
{

AprscFeed::AprscFeed(const std::string& cr_name, std::int32_t prio,
        const std::unordered_map<std::string, std::string>& cr_kvmap)
        : Feed(cr_name, prio, cr_kvmap)
{
    auto it = mKvMap.find(KV_KEY_LOGIN);
    if (it == mKvMap.end())
    {
        Logger::warn("(AprscFeed) could not find: ", mName + "." KV_KEY_LOGIN);
        throw std::logic_error("No login given");
    } else
    {
        mpParser = std::unique_ptr<parser::Parser>(new parser::AprsParser());
        mpClient = std::unique_ptr<tcp::client::Client>(
                new tcp::client::AprscClient(mKvMap.find(KV_KEY_HOST)->second,
                        mKvMap.find(KV_KEY_PORT)->second, it->second, *this));
    }
}

AprscFeed::~AprscFeed() noexcept
{
}

} // namespace feed
