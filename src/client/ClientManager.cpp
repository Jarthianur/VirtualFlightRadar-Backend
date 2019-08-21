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

#include "client/ClientManager.h"

#include "client/ClientFactory.h"
#include "feed/Feed.h"

namespace client
{
ClientManager::~ClientManager() noexcept
{
    stop();
}

void ClientManager::subscribe(std::shared_ptr<feed::Feed> feed)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    ClientIter                  it = m_clients.end();
    it                             = m_clients.insert(ClientFactory::createClientFor(feed)).first;
    if (it != m_clients.end())
    {
        (*it)->subscribe(feed);
    }
    else
    {
        throw std::logic_error("could not subscribe feed " + feed->name);
    }
}

void ClientManager::run()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto it : m_clients)
    {
        m_thdGroup.create_thread([this, it] {
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
        for (auto it : m_clients)
        {
            it->scheduleStop();
        }
    }
    m_thdGroup.join_all();
}
}  // namespace client
