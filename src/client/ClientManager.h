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

#pragma once

#include <algorithm>
#include <cstddef>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_set>

#include "Client.h"

namespace feed
{
class Feed;
}  // namespace feed

struct thread_group
{
    void create_thread(const std::function<void()>& func)
    {
        _threads.push_back(std::thread(func));
    }

    void join_all()
    {
        std::for_each(_threads.begin(), _threads.end(), [](std::thread& thd) {
            if (thd.joinable())
            {
                thd.join();
            }
        });
    }

private:
    std::list<std::thread> _threads;
};

namespace client
{
struct ClientHasher
{
    std::size_t operator()(const std::shared_ptr<Client>& client) const
    {
        return client->hash();
    }
};

struct ClientComparator
{
    bool operator()(const std::shared_ptr<Client>& client1,
                    const std::shared_ptr<Client>& client2) const
    {
        return client1->equals(*client2);
    }
};

using ClientSet = std::unordered_set<std::shared_ptr<Client>, ClientHasher, ClientComparator>;

using ClientIter = ClientSet::iterator;

class ClientManager
{
public:
    ClientManager();

    ~ClientManager() noexcept;

    void subscribe(std::shared_ptr<feed::Feed> feed);

    void run();

    void stop();

private:
    ClientSet m_clients;

    thread_group m_thdGroup;

    mutable std::mutex m_mutex;
};

}  // namespace client
