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

#include "../VFRB.h"
#include "../config/Configuration.h"
#include "../network/client/Client.h"
#include "../util/Logger.h"

using namespace util;

namespace feed
{
Feed::Feed(const std::string& crName, const config::KeyValueMap& crKvMap)
    : mName(crName), mKvMap(crKvMap)
{
    initPriority();
    if(mKvMap.find(KV_KEY_HOST) == mKvMap.end())
    {
        Logger::warn({"(Feed) could not find: ", mName, "." KV_KEY_HOST});
        throw std::logic_error("No host given");
    }
    if(mKvMap.find(KV_KEY_PORT) == mKvMap.end())
    {
        Logger::warn({"(Feed) could not find: ", mName, "." KV_KEY_PORT});
        throw std::logic_error("No port given");
    }
}

Feed::~Feed() noexcept
{}

Feed::Feed(BOOST_RV_REF(Feed) other)
    : mName(std::move(other.mName)),
      mKvMap(std::move(other.mKvMap)),
      mpClient(std::move(other.mpClient)),
      mPriority(other.mPriority)
{}

Feed& Feed::operator=(BOOST_RV_REF(Feed))
{
    return *this;
}

void Feed::run(boost::asio::signal_set& rSigset) noexcept
{
    if(VFRB::global_run_status)
    {
        mpClient->run(rSigset);
    }
}

const std::string& Feed::getName() const
{
    return mName;
}

std::uint32_t Feed::getPriority() const
{
    return mPriority;
}

void Feed::initPriority()
{
    std::uint64_t priority = 0;
    try
    {
        priority = std::stoul(mKvMap.at(KV_KEY_PRIORITY));
        if(priority > std::numeric_limits<std::uint32_t>::max())
        {
            priority = std::numeric_limits<std::uint32_t>::max();
        }
    }
    catch(const std::logic_error&)
    {
        Logger::warn({"(Config) create feed ", mName, ": Invalid priority given."});
    }
    if(priority == 0)
    {
        Logger::warn({"(Config) create feed " , mName,
                     ": Priority is 0; this feed cannot update higher ones."});
    }
    mPriority = static_cast<std::uint32_t>(priority);
}

}  // namespace feed
