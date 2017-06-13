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

#include "../config/Configuration.h"
#include "../parser/AprsParser.h"
#include "../parser/GpsParser.h"
#include "../parser/SbsParser.h"
#include "../parser/SensorParser.h"
#include "../tcp/client/AprscClient.h"
#include "../tcp/client/GpsdClient.h"
#include "../tcp/client/SbsClient.h"
#include "../tcp/client/SensorClient.h"
#include "../util/Logger.h"
#include "VFRB.h"

using namespace util;

namespace vfrb
{

Feed::Feed(const std::string& cr_name, std::int32_t prio, InputType type,
           const std::unordered_map<std::string, std::string>& cr_kvmap)
        : mName(cr_name),
          mPriority(prio),
          mType(type),
          mKvMap(cr_kvmap)
{
}

Feed::~Feed() noexcept
{
}

Feed::Feed(BOOST_RV_REF(Feed) other)
: mName(std::move(other.mName)),
mPriority(other.mPriority),
mType(other.mType),
mKvMap(std::move(other.mKvMap))
{
}

Feed& Feed::operator =(BOOST_RV_REF(Feed))
{
    return *this;
}

void Feed::run(boost::asio::signal_set& r_sigset) noexcept
{
    std::string host, port;
    auto it = mKvMap.find(KV_KEY_HOST);
    if (it != mKvMap.end())
    {
        host = it->second;
    } else
    {
        Logger::warn("(Feed) could not find: ", mName + "." KV_KEY_HOST);
        return;
    }
    it = mKvMap.find(KV_KEY_PORT);
    if (it != mKvMap.end())
    {
        port = it->second;
    } else
    {
        Logger::warn("(Feed) could not find: ", mName + "." KV_KEY_PORT);
        return;
    }
    switch (mType)
    {
        case InputType::APRSC:
        {
            std::string login;
            auto it = mKvMap.find(KV_KEY_LOGIN);
            if (it != mKvMap.end())
            {
                login = it->second;
            } else
            {
                Logger::warn("(Feed) could not find: ",
                        mName + "." KV_KEY_LOGIN);
                return;
            }
            mpParser = std::unique_ptr<parser::Parser>(
                    new parser::AprsParser());
            mpClient = std::unique_ptr<tcp::client::Client>(
                    new tcp::client::AprscClient(r_sigset, host, port, login,
                            *this));
            break;
        }
        case InputType::SBS:
        {
            mpParser = std::unique_ptr<parser::Parser>(new parser::SbsParser());
            mpClient = std::unique_ptr<tcp::client::Client>(
                    new tcp::client::SbsClient(r_sigset, host, port, *this));
            break;
        }
        case InputType::SENSOR:
        {
            mpParser = std::unique_ptr<parser::Parser>(
                    new parser::SensorParser());
            mpClient = std::unique_ptr<tcp::client::Client>(
                    new tcp::client::SensorClient(r_sigset, host, port, *this));
            break;
        }
        case InputType::GPS:
        {
            mpParser = std::unique_ptr<parser::Parser>(new parser::GpsParser());
            mpClient = std::unique_ptr<tcp::client::Client>(
                    new tcp::client::GpsdClient(r_sigset, host, port, *this));
            break;
        }
        default:
            return;
    }
    if (VFRB::global_run_status)
    {
        mpClient->run();
    }
}

std::int32_t Feed::process(const std::string& cr_res) noexcept
{
    return mpParser->unpack(cr_res, mPriority);
}

}  // namespace vfrb
