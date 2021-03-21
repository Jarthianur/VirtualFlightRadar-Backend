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

#include "concurrent/CThreadGroup.hpp"
#include "concurrent/Mutex.hpp"
#include "error/IError.hpp"

#include "IClient.hpp"

namespace vfrb::feed
{
class IFeed;
}  // namespace vfrb::feed

namespace vfrb::client
{
struct SClientHasher
{
    auto
    operator()(Shared<IClient> const& c_) const -> usize {
        return c_->Hash();
    }
};

struct SClientComparator
{
    auto
    operator()(Shared<IClient> const& c1_, Shared<IClient> const& c2_) const -> bool {
        return c1_->Equals(*c2_);
    }
};

using ClientSet  = Set<Shared<IClient>, SClientHasher, SClientComparator>;
using ClientIter = ClientSet::iterator;

class CClientManager
{
    NOT_COPYABLE(CClientManager)
    NOT_MOVABLE(CClientManager)

    concurrent::Mutex mutable m_mutex;
    ClientSet                GUARDED_BY(m_mutex) m_clients;
    concurrent::CThreadGroup GUARDED_BY(m_mutex) m_thdGroup;

public:
    CClientManager() = default;
    ~CClientManager() noexcept;

    void
    Subscribe(Shared<feed::IFeed> feed_) REQUIRES(!m_mutex);

    void
    Run() REQUIRES(!m_mutex);

    void
    Stop() REQUIRES(!m_mutex);
};

namespace error
{
class CFeedSubscriptionError : public vfrb::error::IError
{
    String const m_msg;

public:
    explicit CFeedSubscriptionError(String const& name_);

    [[nodiscard]] auto
    Message() const noexcept -> str override;
};
}  // namespace error
}  // namespace vfrb::client
