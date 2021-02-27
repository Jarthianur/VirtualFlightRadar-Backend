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

#include "concurrent/Mutex.hpp"
#include "net/IConnector.hpp"
#include "net/SEndpoint.hpp"

#include "CTimeoutBackoff.hpp"

namespace vfrb::feed
{
class IFeed;
}  // namespace vfrb::feed
namespace vfrb::client
{
class IClient
{
    NOT_COPYABLE(IClient)
    NOT_MOVABLE(IClient)

protected:
    enum class EState : enum_type
    {
        NONE,
        CONNECTING,
        RUNNING,
        STOPPING
    };

    concurrent::Mutex mutable m_mutex;
    CTimeoutBackoff             GUARDED_BY(m_mutex) m_backoff;
    Shared<net::IConnector>     GUARDED_BY(m_mutex) m_connector;
    EState                      GUARDED_BY(m_mutex) m_state = EState::NONE;
    net::SEndpoint const        m_endpoint;
    Vector<Shared<feed::IFeed>> GUARDED_BY(m_mutex) m_feeds;

    IClient(net::SEndpoint const& ep_, Shared<net::IConnector> con_);

    virtual void
    handleConnect(Result<void> res_) = 0;

    virtual auto
    logPrefix() const -> str = 0;

    virtual void
    stop() REQUIRES(m_mutex);

    virtual void
    read() REQUIRES(m_mutex);

    void
    timedConnect() REQUIRES(m_mutex);

    void
    connect() REQUIRES(m_mutex);

    void
    reconnect() REQUIRES(m_mutex);

    void
    handleTimedConnect(Result<void> res_) REQUIRES(!m_mutex);

    void
    handleRead(Result<String>&& res_) REQUIRES(!m_mutex);

public:
    virtual ~IClient() noexcept = default;

    void
    Run() REQUIRES(!m_mutex);

    void
    ScheduleStop() REQUIRES(!m_mutex);

    void
    Subscribe(Shared<feed::IFeed> feed_) REQUIRES(!m_mutex);

    virtual auto
    Equals(IClient const& other_) const -> bool;

    virtual auto
    Hash() const -> usize;
};
}  // namespace vfrb::client
