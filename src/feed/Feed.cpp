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

#include "../config/Configuration.h"
#include "../network/client/Client.h"
#include "../util/Logger.h"
#include "../VFRB.h"

using namespace util;

namespace feed
{

Feed::Feed(const std::string& crName, std::uint32_t vPriority,
           const config::keyValueMap& crKvMap)
        : mName(crName),
          mPriority(vPriority),
          mKvMap(crKvMap)
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

void Feed::run(boost::asio::signal_set& rSigset) noexcept
{
    if (VFRB::global_run_status)
    {
        mpClient->run(rSigset);
    }
}

}  // namespace feed
