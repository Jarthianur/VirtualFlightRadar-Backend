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

#include <vector>

#include "concurrent/Mutex.hpp"
#include "net/Connector.hpp"
#include "net/Endpoint.hpp"
#include "util/class_utils.h"

#include "TimeoutBackoff.h"
#include "types.h"

namespace vfrb::feed
{
class Feed;
}  // namespace vfrb::feed
namespace vfrb::client
{
/**
 * @brief Base class for an async TCP client
 */
class Client
{
    NOT_COPYABLE(Client)

protected:
    enum class State : enum_type
    {
        NONE,
        CONNECTING,
        RUNNING,
        STOPPING
    };

    concurrent::Mutex mutable m_mutex;
    TimeoutBackoff                 GUARDED_BY(m_mutex) m_backoff;
    s_ptr<net::Connector>          GUARDED_BY(m_mutex) m_connector;            /// Connector interface
    State                          GUARDED_BY(m_mutex) m_state = State::NONE;  /// Run state indicator
    net::Endpoint const            m_endpoint;                                 /// Remote endpoint
    std::vector<s_ptr<feed::Feed>> GUARDED_BY(m_mutex) m_feeds;  /// Container for subscribed feeds

    /**
     * @param endpoint  The connection Endpoint
     * @param component The component name
     * @param connector The Connector interface
     */
    Client(net::Endpoint const& endpoint, SPtr<net::Connector> connector);

    /**
     * @brief Handler for connect
     * @param error The error indicator
     */
    virtual void handleConnect(net::ErrorCode error) = 0;

    virtual char const* logPrefix() const = 0;

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
    void handleTimedConnect(net::ErrorCode error) REQUIRES(!m_mutex);

    /**
     * @brief Handler for read
     * @param error    The error indicator
     * @param response The received string
     */
    void handleRead(net::ErrorCode, str const& response) REQUIRES(!m_mutex);

public:
    virtual ~Client() noexcept = default;

    /**
     * @brief Run the clients connection- and eventhandlers.
     * @note Returns after all queued handlers have returned.
     * @threadsafe
     */
    void run() REQUIRES(!m_mutex);

    /**
     * @brief Stop after client has been started.
     * @note Wait until run has been called.
     * @threadsafe
     */
    void scheduleStop() REQUIRES(!m_mutex);

    /**
     * @brief Subscribe a Feed to this client.
     * @param feed The Feed to subscribe
     * @threadsafe
     */
    void subscribe(s_ptr<feed::Feed> feed) REQUIRES(!m_mutex);

    /**
     * @brief Compare to another client by value.
     * @param other The other Client
     * @return true if equals, else false
     */
    virtual bool equals(Client const& other) const;

    /**
     * @brief Get a hash value of this client.
     * @return the hash value
     */
    virtual usize hash() const;
};
}  // namespace vfrb::client
