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
#include "../Feed.h"

namespace feed
{
namespace client
{
ClientManager::ClientManager()
{}

ClientManager::~ClientManager() noexcept
{}

void ClientManager::subscribe(Feed& rFeed, const Endpoint& crEndpoint, Protocol vProtocol)
{
    auto it = mClients.find(crEndpoint);
    if(it == mClients.end())
    {
        switch(vProtocol)
        {
            case Protocol::APRS:
            it = mClients.insert({crEndpoint,std::make_unique<Client>(crEndpoint.getAddress(), crEndpoint.getPort(),)});
                break;
            case Protocol::SBS:
                break;
            case Protocol::GPS:
                break;
            case Protocol::SENS:
                break;
        }
    }
    it->second->mrFeeds.push_back(rFeed);
}

}  // namespace client
}  // namespace feed
