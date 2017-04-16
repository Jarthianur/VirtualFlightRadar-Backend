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

#include "../config/Configuration.h"
#include "../parser/APRSParser.h"
#include "../parser/SBSParser.h"
#include "../parser/SensorParser.h"
#include "../tcp/client/APRSCClient.h"
#include "../tcp/client/SBSClient.h"
#include "../tcp/client/SensorClient.h"
#include "../util/Logger.h"

Feed::Feed(const std::string& name, Priority prio, InputType type,
           const std::unordered_map<std::string, std::string>& kvmap)
        : mName(name),
          mPriority(prio),
          mType(type),
          mKVmap(kvmap)
{
}

Feed::~Feed() noexcept
{
}

Feed::Feed(BOOST_RV_REF(Feed) other)
: mName(other.mName),
mPriority(other.mPriority),
mType(other.mType),
mKVmap(other.mKVmap)
{
}

Feed& Feed::operator =(BOOST_RV_REF(Feed))
{
    return *this;
}

void Feed::run(boost::asio::signal_set& sigset)
{
    std::string host, port;
    auto it = mKVmap.find(KV_KEY_HOST);
    if (it != mKVmap.end())
    {
        host = it->second;
    }
    else
    {
        Logger::warn("(Feed) could not find: ", mName + "." KV_KEY_HOST);
        return;
    }
    it = mKVmap.find(KV_KEY_PORT);
    if (it != mKVmap.end())
    {
        port = it->second;
    }
    else
    {
        Logger::warn("(Feed) could not find: ", mName + "." KV_KEY_PORT);
        return;
    }
    switch (mType)
    {
        case InputType::APRSC:
        {
            std::string login;
            auto it = mKVmap.find(KV_KEY_LOGIN);
            if (it != mKVmap.end())
            {
                login = it->second;
            }
            else
            {
                Logger::warn("(Feed) could not find: ", mName + "." KV_KEY_LOGIN);
                return;
            }
            mpParser = std::unique_ptr<Parser>(new APRSParser());
            mpClient = std::unique_ptr<Client>(
                    new APRSCClient(sigset, host, port, login, *this));
            break;
        }
        case InputType::SBS:
        {
            mpParser = std::unique_ptr<Parser>(new SBSParser());
            mpClient = std::unique_ptr<Client>(new SBSClient(sigset, host, port, *this));
            break;
        }
        case InputType::SENSOR:
        {
            mpParser = std::unique_ptr<Parser>(new SensorParser());
            mpClient = std::unique_ptr<Client>(
                    new SensorClient(sigset, host, port, *this));
            break;
        }
            /*case InputType::GPS:{
             mpParser = std::unique_ptr<Parser>(new GPSParser());
             mClient = std::unique_ptr<Client>(
             new GPSClient(sigset, host, port, *this));
             break;}*/
        default:
            return;
    }
    mpClient->run();
}

void Feed::process(const std::string& data) noexcept
{
    mpParser->unpack(data, mPriority);
}
