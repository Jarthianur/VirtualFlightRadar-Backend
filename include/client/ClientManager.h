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
class IFeed;
}  // namespace vfrb::feed

namespace vfrb::client
{
/**
 * @brief Functor for hashing clients.
 */
struct SClientHasher
{
    usize operator()(SPtr<IClient> const& c_) const
    {
        return c_->Hash();
    }
};

/**
 * @brief Functor for comparing clients.
 */
struct SClientComparator
{
    bool operator()(SPtr<IClient> const& c1_, SPtr<IClient> const& c2_) const
    {
        return c1_->Equals(*c2_);
    }
};

/// Set of clients with custom hasher and comparator
using ClientSet = std::unordered_set<SPtr<IClient>, SClientHasher, SClientComparator>;
/// Iterator in ClientSet
using ClientIter = ClientSet::iterator;

/**
 * @brief Managing multi-threaded clients.
 */
class CClientManager
{
    NOT_COPYABLE(CClientManager)

    concurrent::Mutex mutable m_mutex;
    ClientSet                GUARDED_BY(m_mutex) m_clients;   ///< Set of clients
    concurrent::CThreadGroup GUARDED_BY(m_mutex) m_thdGroup;  ///< Thread group for client threads

public:
    CClientManager() = default;
    ~CClientManager() noexcept;

    /**
     * @brief Subscribe a Feed to the respective Client.
     * @param feed The feed to subscribe
     * @threadsafe
     */
    void Subscribe(SPtr<feed::IFeed> feed_) REQUIRES(!m_mutex);

    /**
     * @brief Run all clients in their own thread.
     * @threadsafe
     */
    void Run() REQUIRES(!m_mutex);

    /**
     * @brief Stop all clients.
     * @note Blocks until all clients have stopped.
     * @threadsafe
     */
    void Stop() REQUIRES(!m_mutex);
};

namespace error
{
class CFeedSubscriptionError : public vfrb::error::IError
{
    Str const m_msg;

public:
    explicit CFeedSubscriptionError(Str const& name_);
    ~CFeedSubscriptionError() noexcept override = default;

    char const* Message() const noexcept override;
};
}  // namespace error
}  // namespace vfrb::client
