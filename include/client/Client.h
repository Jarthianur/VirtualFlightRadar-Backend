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
#include <vector>

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
    enum class State : enum_t
    {
        NONE,
        CONNECTING,
        RUNNING,
        STOPPING
    };

    TimeoutBackoff                 m_backoff;
    s_ptr<net::Connector>          m_connector;            /// Connector interface
    State                          m_state = State::NONE;  /// Run state indicator
    net::Endpoint const            m_endpoint;             /// Remote endpoint
    std::vector<s_ptr<feed::Feed>> m_feeds;                /// Container for subscribed feeds
    std::mutex mutable m_mutex;

    /**
     * @param endpoint  The connection Endpoint
     * @param component The component name
     * @param connector The Connector interface
     */
    Client(net::Endpoint const& endpoint, s_ptr<net::Connector> connector);

    /**
     * @brief Handler for connect
     * @param error The error indicator
     */
    virtual void handleConnect(net::ErrorCode error) = 0;

    virtual char const* logPrefix() const = 0;

    /**
     * @brief Stop and close the connection.
     */
    virtual void stop();

    /**
     * @brief Schedule to read from endpoint.
     */
    virtual void read();

    /**
     * @brief Schedule to connect after some timeout.
     */
    void timedConnect();

    /**
     * @brief Schedule to connect to endpoint.
     */
    void connect();

    /**
     * @brief Close connection and schedule to connect after timeout.
     * @threadsafe
     */
    void reconnect();

    /**
     * @brief Handler for timedConnect
     * @param error The error indicator
     */
    void handleTimedConnect(net::ErrorCode error);

    /**
     * @brief Handler for read
     * @param error    The error indicator
     * @param response The received string
     */
    void handleRead(net::ErrorCode, str const& response);

public:
    virtual ~Client() noexcept = default;

    /**
     * @brief Run the clients connection- and eventhandlers.
     * @note Returns after all queued handlers have returned.
     * @threadsafe
     */
    void run();

    /**
     * @brief Stop after client has been started.
     * @note Wait until run has been called.
     * @threadsafe
     */
    void scheduleStop();

    /**
     * @brief Subscribe a Feed to this client.
     * @param feed The Feed to subscribe
     * @threadsafe
     */
    void subscribe(s_ptr<feed::Feed> feed);

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
