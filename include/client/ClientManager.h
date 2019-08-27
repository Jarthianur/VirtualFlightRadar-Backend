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

#include <mutex>
#include <unordered_set>

#include "concurrency/ThreadGroup.hpp"
#include "util/defines.h"
#include "util/types.h"

#include "Client.h"

namespace vfrb::feed
{
class Feed;
}  // namespace vfrb::feed

namespace vfrb::client
{
/**
 * @brief Functor for hashing clients.
 */
struct ClientHasher
{
    usize operator()(s_ptr<Client> const& client) const
    {
        return client->hash();
    }
};

/**
 * @brief Functor for comparing clients.
 */
struct ClientComparator
{
    bool operator()(s_ptr<Client> const& client1, s_ptr<Client> const& client2) const
    {
        return client1->equals(*client2);
    }
};

/// Set of clients with custom hasher and comparator
using ClientSet = std::unordered_set<s_ptr<Client>, ClientHasher, ClientComparator>;
/// Iterator in ClientSet
using ClientIter = ClientSet::iterator;

/**
 * @brief Managing multi-threaded clients.
 */
class ClientManager
{
    NOT_COPYABLE(ClientManager)

    ClientSet                m_clients;   ///< Set of clients
    concurrency::ThreadGroup m_thdGroup;  ///< Thread group for client threads
    mutable std::mutex       m_mutex;

public:
    ClientManager() = default;
    ~ClientManager() noexcept;

    /**
     * @brief Subscribe a Feed to the respective Client.
     * @param feed The feed to subscribe
     * @threadsafe
     */
    void subscribe(s_ptr<feed::Feed> feed);

    /**
     * @brief Run all clients in their own thread.
     * @threadsafe
     */
    void run();

    /**
     * @brief Stop all clients.
     * @note Blocks until all clients have stopped.
     * @threadsafe
     */
    void stop();
};
}  // namespace vfrb::client
