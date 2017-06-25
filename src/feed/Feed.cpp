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

#include "Feed.h"

#include <algorithm>
#include <atomic>
#include <stdexcept>
#include <unordered_map>

#include "../tcp/client/Client.h"
#include "../VFRB.h"
#include "../config/Configuration.h"
#include "../util/Logger.h"

using namespace util;

namespace feed
{

Feed::Feed(const std::string& cr_name, std::int32_t prio,
        const config::keyValueMap& cr_kvmap)
        : mName(cr_name),
          mPriority(prio),
          mKvMap(cr_kvmap)
{
    if (mKvMap.find(KV_KEY_HOST) == mKvMap.end())
    {
        Logger::warn("(Feed) could not find: ", mName + "." KV_KEY_HOST);
        throw std::logic_error("No host given");
    }
    if (mKvMap.find(KV_KEY_PORT) == mKvMap.end())
    {
        Logger::warn("(Feed) could not find: ", mName + "." KV_KEY_PORT);
        throw std::logic_error("No port given");
    }
}

Feed::~Feed() noexcept
{
}

Feed::Feed(BOOST_RV_REF(Feed) other)
: mName(std::move(other.mName)),
mPriority(other.mPriority),
mKvMap(std::move(other.mKvMap)),
mpClient(std::move(other.mpClient))
{
}

Feed& Feed::operator =(BOOST_RV_REF(Feed))
{
    return *this;
}

void Feed::run(boost::asio::signal_set& r_sigset) noexcept
{
    if (VFRB::global_run_status)
    {
        mpClient->run(r_sigset);
    }
}

}  // namespace feed
