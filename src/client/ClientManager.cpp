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

#include "../feed/AprscFeed.h"
#include "../feed/Feed.h"
#include "AprscClient.h"
#include "ConnectorImplBoost.h"
#include "GpsdClient.h"
#include "SbsClient.h"
#include "SensorClient.h"

namespace client
{
ClientManager::ClientManager()
{}

ClientManager::~ClientManager() noexcept
{
    stop();
}

void ClientManager::subscribe(std::shared_ptr<feed::Feed> feed)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    ClientIter it     = m_clients.end();
    Endpoint endpoint = feed->get_endpoint();
    std::shared_ptr<Connector> connector    = std::make_shared<ConnectorImplBoost>();
    switch(feed->get_protocol())
    {
        case feed::Feed::Protocol::APRS:
            it = m_clients
                     .insert(std::make_shared<AprscClient>(
                         endpoint, std::static_pointer_cast<feed::AprscFeed>(feed)->get_login(),
                         connector))
                     .first;
            break;
        case feed::Feed::Protocol::SBS:
            it = m_clients.insert(std::make_shared<SbsClient>(endpoint, connector)).first;
            break;
        case feed::Feed::Protocol::GPS:
            it = m_clients.insert(std::make_shared<GpsdClient>(endpoint, connector)).first;
            break;
        case feed::Feed::Protocol::SENSOR:
            it = m_clients.insert(std::make_shared<SensorClient>(endpoint, connector)).first;
            break;
    }
    (*it)->subscribe(feed);
}

void ClientManager::run()
{std::lock_guard<std::mutex> lock(m_mutex);
    for(auto it : m_clients)
    {
        m_thdGroup.create_thread([this,it] {
            it->run();
            std::lock_guard<std::mutex> lock(m_mutex);
            m_clients.erase(it);
        });
    }
}

void ClientManager::stop()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for(auto it : m_clients)
        {
            it->scheduleStop();
        }
    }
    m_thdGroup.join_all();
}

}  // namespace client
