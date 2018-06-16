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

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "../AprscFeed.h"
#include "../Feed.h"
#include "AprscClient.h"
#include "ClientManager.h"
#include "GpsdClient.h"
#include "SbsClient.h"
#include "SensorClient.h"

namespace feed
{
namespace client
{
ClientManager::ClientManager()
{}

ClientManager::~ClientManager() noexcept
{}

ClientSet::iterator ClientManager::subscribe(std::shared_ptr<Feed> rpFeed,
                                             const Endpoint& crEndpoint,
                                             Protocol vProtocol)
{
    ClientSet::iterator it;
    switch(vProtocol)
    {
        case Protocol::APRS:
        {
            if(AprscFeed* feed = dynamic_cast<AprscFeed*>(rpFeed.get()))
            {
                it = mClients
                         .insert(std::make_unique<AprscClient>(crEndpoint,
                                                               feed->getLoginStr()))
                         .first;
            }
            else
            {
                throw std::invalid_argument("wrong protocol");
            }
        }
        break;
        case Protocol::SBS:
            it = mClients.insert(std::make_unique<SbsClient>(crEndpoint)).first;
            break;
        case Protocol::GPS:
            it = mClients.insert(std::make_unique<GpsdClient>(crEndpoint)).first;
            break;
        case Protocol::SENSOR:
            it = mClients.insert(std::make_unique<SensorClient>(crEndpoint)).first;
            break;
    }
    (*it)->subscribe(rpFeed);
    return it;
}

void ClientManager::run(boost::thread_group& rThdGroup, boost::asio::signal_set& rSigset)
{
    for(auto& it : mClients)
    {
        rThdGroup.create_thread([&]() { it->run(rSigset); });
    }
}

}  // namespace client
}  // namespace feed
