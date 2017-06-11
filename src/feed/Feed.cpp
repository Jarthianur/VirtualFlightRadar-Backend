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

#include <boost/thread/lock_types.hpp>
#include <algorithm>
#include <stdexcept>

#include "../config/Configuration.h"
#include "../util/Logger.h"
#include "../VFRB.h"

using namespace util;

namespace feed
{

Feed::Feed(const std::string& cr_name, std::int32_t prio,
           const std::unordered_map<std::string, std::string>& cr_kvmap)
        : mName(cr_name),
          mPriority(prio),
          mKVmap(cr_kvmap)
{
    if (mKVmap.find(KV_KEY_HOST) == mKVmap.end())
    {
        Logger::warn("(Feed) could not find: ", mName + "." KV_KEY_HOST);
        throw std::runtime_error("No host given");
    }
    if (mKVmap.find(KV_KEY_PORT) == mKVmap.end())
    {
        Logger::warn("(Feed) could not find: ", mName + "." KV_KEY_PORT);
        throw std::runtime_error("No port given");
    }
}

Feed::~Feed() noexcept
{
}

Feed::Feed(BOOST_RV_REF(Feed) other)
: mName(std::move(other.mName)),
mPriority(other.mPriority),
mKVmap(std::move(other.mKVmap)),
mpClient(std::move(other.mpClient)),
mpParser(std::move(other.mpParser))
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

std::int32_t Feed::process(const std::string& cr_res) noexcept
{
    return mpParser->unpack(cr_res, mPriority);
}

}  // namespace feed
