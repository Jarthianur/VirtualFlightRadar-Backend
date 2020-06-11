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

#include <vector>

#include "concurrent/Mutex.hpp"
#include "net/IConnector.hpp"
#include "net/SEndpoint.hpp"
#include "util/ClassUtils.hpp"

#include "CTimeoutBackoff.hpp"
#include "Types.hpp"

namespace vfrb::feed
{
class IFeed;
}  // namespace vfrb::feed
namespace vfrb::client
{
/**
 * @brief Base class for an async TCP client
 */
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
    CTimeoutBackoff                GUARDED_BY(m_mutex) m_backoff;
    SPtr<net::IConnector>          GUARDED_BY(m_mutex) m_connector;             /// Connector interface
    EState                         GUARDED_BY(m_mutex) m_state = EState::NONE;  /// Run state indicator
    net::SEndpoint const           m_endpoint;                                  /// Remote endpoint
    std::vector<SPtr<feed::IFeed>> GUARDED_BY(m_mutex) m_feeds;  /// Container for subscribed feeds

    /**
     * @param endpoint  The connection Endpoint
     * @param component The component name
     * @param connector The Connector interface
     */
    IClient(net::SEndpoint const& ep_, SPtr<net::IConnector> con_);

    /**
     * @brief Handler for connect
     * @param error The error indicator
     */
    virtual void handleConnect(net::EErrc err_) = 0;

    virtual auto logPrefix() const -> str = 0;

    /**
     * @brief Stop and close the connection.
     */
    virtual void stop() REQUIRES(m_mutex);

    /**
     * @brief Schedule to read from endpoint.
     */
    virtual void read() REQUIRES(m_mutex);

    /**
     * @brief Schedule to connect after some timeout.
     */
    void timedConnect() REQUIRES(m_mutex);

    /**
     * @brief Schedule to connect to endpoint.
     */
    void connect() REQUIRES(m_mutex);

    /**
     * @brief Close connection and schedule to connect after timeout.
     * @threadsafe
     */
    void reconnect() REQUIRES(m_mutex);

    /**
     * @brief Handler for timedConnect
     * @param error The error indicator
     */
    void handleTimedConnect(net::EErrc err_) REQUIRES(!m_mutex);

    /**
     * @brief Handler for read
     * @param error    The error indicator
     * @param response The received string
     */
    void handleRead(net::EErrc, String const& str_) REQUIRES(!m_mutex);

public:
    virtual ~IClient() noexcept = default;

    /**
     * @brief Run the clients connection- and eventhandlers.
     * @note Returns after all queued handlers have returned.
     * @threadsafe
     */
    void Run() REQUIRES(!m_mutex);

    /**
     * @brief Stop after client has been started.
     * @note Wait until run has been called.
     * @threadsafe
     */
    void ScheduleStop() REQUIRES(!m_mutex);

    /**
     * @brief Subscribe a Feed to this client.
     * @param feed The Feed to subscribe
     * @threadsafe
     */
    void Subscribe(SPtr<feed::IFeed> feed_) REQUIRES(!m_mutex);

    /**
     * @brief Compare to another client by value.
     * @param other The other Client
     * @return true if equals, else false
     */
    virtual auto Equals(IClient const& other_) const -> bool;

    /**
     * @brief Get a hash value of this client.
     * @return the hash value
     */
    virtual auto Hash() const -> usize;
};
}  // namespace vfrb::client
