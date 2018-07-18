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

#include "ClientManager.h"

#include <stdexcept>
#include <boost/thread/lock_guard.hpp>

#include "../AprscFeed.h"
#include "../Feed.h"
#include "AprscClient.hpp"
#include "GpsdClient.h"
#include "SbsClient.hpp"
#include "SensorClient.h"

namespace feed
{
namespace client
{
ClientManager::ClientManager()
{}

ClientManager::~ClientManager() noexcept
{}

void ClientManager::subscribe(std::shared_ptr<Feed> rpFeed, const Endpoint& crEndpoint,
                              Protocol vProtocol)
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    ClientSet::iterator it = mClients.end();
    switch(vProtocol)
    {
        case Protocol::APRS:
        {
            if(std::shared_ptr<AprscFeed> feed = std::dynamic_pointer_cast<AprscFeed>(rpFeed))
            {
                it = mClients.insert(std::make_shared<AprscClient>(crEndpoint, feed->getLoginStr()))
                         .first;
            }
            else
            {
                throw std::invalid_argument("wrong protocol");
            }
        }
        break;
        case Protocol::SBS:
            it = mClients.insert(std::make_shared<SbsClient>(crEndpoint)).first;
            break;
        case Protocol::GPS:
            it = mClients.insert(std::make_shared<GpsdClient>(crEndpoint)).first;
            break;
        case Protocol::SENSOR:
            it = mClients.insert(std::make_shared<SensorClient>(crEndpoint)).first;
            break;
    }
    (*it)->subscribe(rpFeed);
}

void ClientManager::run()
{
    for(auto& it : mClients)
    {
        mThdGroup.create_thread([&]() {
            it->run();
            boost::lock_guard<boost::mutex> lock(mMutex);
            mClients.erase(it);
        });
    }
}

void ClientManager::stop()
{
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        for(const auto& it : mClients)
        {
            it->lockAndStop();
        }
    }
    mThdGroup.join_all();
}

}  // namespace client
}  // namespace feed
