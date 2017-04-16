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

#include <boost/asio/signal_set.hpp>

#include "../tcp/client/APRSCClient.h"
#include "../tcp/client/SBSClient.h"
#include "../tcp/client/SensorClient.h"

Feed::Feed(const std::string& name, std::uint32_t prio, InputType type,
           std::unordered_map<std::string, std::string>& kvmap)
        : mName(name),
          mPriority(prio),
          mType(type),
          mKVmap(kvmap)
{
}

Feed::~Feed() noexcept
{
}

void Feed::run(boost::asio::signal_set& sigset)
{
    switch (mType)
    {
        case Feed::InputType::APRSC:
            mClient = std::unique_ptr<Client>(
                    new APRSCClient(sigset, mKVmap.find("host"), mKVmap.find("port"),
                                    mKVmap.find("login")));
            break;
        case Feed::InputType::SBS:
            mClient = std::unique_ptr<Client>(
                    new SBSClient(sigset, mKVmap.find("host"), mKVmap.find("port")));
            break;
        case Feed::InputType::SENSOR:
            mClient = std::unique_ptr<Client>(
                    new SensorClient(sigset, mKVmap.find("host"), mKVmap.find("port")));
            break;
            /*case Feed::InputType::GPS:
             mClient = std::unique_ptr<Client>(
             new GPSClient(sigset, mKVmap.find("host"), mKVmap.find("port")));
             break;*/
        default:
            return;
    }
    mClient->run();
}
