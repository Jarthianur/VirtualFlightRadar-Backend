/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <unordered_set>

#include "concurrent/CThreadGroup.hpp"
#include "concurrent/Mutex.hpp"
#include "error/IError.hpp"
#include "util/ClassUtils.hpp"

#include "IClient.hpp"
#include "Types.hpp"

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
    auto operator()(SPtr<IClient> const& c_) const -> usize {
        return c_->Hash();
    }
};

/**
 * @brief Functor for comparing clients.
 */
struct SClientComparator
{
    auto operator()(SPtr<IClient> const& c1_, SPtr<IClient> const& c2_) const -> bool {
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
    NOT_MOVABLE(CClientManager)

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
    String const m_msg;

public:
    explicit CFeedSubscriptionError(String const& name_);

    [[nodiscard]] auto Message() const noexcept -> str override;
};
}  // namespace error
}  // namespace vfrb::client
