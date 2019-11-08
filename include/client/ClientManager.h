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

#include <unordered_set>

#include "concurrent/Mutex.hpp"
#include "concurrent/ThreadGroup.hpp"
#include "error/Error.hpp"
#include "util/class_utils.h"

#include "Client.h"
#include "types.h"

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
    usize operator()(SPtr<Client> const& client) const
    {
        return client->hash();
    }
};

/**
 * @brief Functor for comparing clients.
 */
struct ClientComparator
{
    bool operator()(SPtr<Client> const& client1, SPtr<Client> const& client2) const
    {
        return client1->equals(*client2);
    }
};

/// Set of clients with custom hasher and comparator
using ClientSet = std::unordered_set<SPtr<Client>, ClientHasher, ClientComparator>;
/// Iterator in ClientSet
using ClientIter = ClientSet::iterator;

/**
 * @brief Managing multi-threaded clients.
 */
class ClientManager
{
    NOT_COPYABLE(ClientManager)

    concurrent::Mutex mutable m_mutex;
    ClientSet               GUARDED_BY(m_mutex) m_clients;   ///< Set of clients
    concurrent::ThreadGroup GUARDED_BY(m_mutex) m_thdGroup;  ///< Thread group for client threads

public:
    ClientManager() = default;
    ~ClientManager() noexcept;

    /**
     * @brief Subscribe a Feed to the respective Client.
     * @param feed The feed to subscribe
     * @threadsafe
     */
    void subscribe(s_ptr<feed::Feed> feed) REQUIRES(!m_mutex);

    /**
     * @brief Run all clients in their own thread.
     * @threadsafe
     */
    void run() REQUIRES(!m_mutex);

    /**
     * @brief Stop all clients.
     * @note Blocks until all clients have stopped.
     * @threadsafe
     */
    void stop() REQUIRES(!m_mutex);
};

namespace error
{
class FeedSubscriptionError : public vfrb::error::IError
{
    Str const m_msg;

public:
    explicit FeedSubscriptionError(Str const& name);
    ~FeedSubscriptionError() noexcept override = default;

    char const* what() const noexcept override;
};
}  // namespace error
}  // namespace vfrb::client
