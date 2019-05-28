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

#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "net/Connector.hpp"
#include "net/Endpoint.hpp"
#include "util/defines.h"

namespace feed
{
class Feed;
}  // namespace feed
namespace client
{
/**
 * @brief Base class for an async TCP client
 */
class Client
{
protected:
    //< begin members >//
    std::shared_ptr<net::Connector>          m_connector;        /// Connector interface
    bool                                     m_running = false;  /// Run state indicator
    const char* const                        m_logPrefix;        /// Component string used for logging
    const net::Endpoint                      m_endpoint;         /// Remote endpoint
    std::vector<std::shared_ptr<feed::Feed>> m_feeds;            /// Container for subscribed feeds
    mutable std::mutex                       m_mutex;
    //< end members >//

    //< begin methods >//
    /**
     * @brief Handler for connect
     * @param error The error indicator
     */
    virtual void handleConnect(bool error) = 0;

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
    void handleTimedConnect(bool error);

    /**
     * @brief Handler for read
     * @param error    The error indicator
     * @param response The received string
     */
    void handleRead(bool error, const std::string& response);
    //< end methods >//

public:
    NOT_COPYABLE(Client)

    /**
     * @param endpoint  The connection Endpoint
     * @param component The component name
     * @param connector The Connector interface
     */
    Client(const net::Endpoint& endpoint, const char* logPrefix, std::shared_ptr<net::Connector> connector);
    virtual ~Client() noexcept = default;

    //< begin interfaces >//
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
    void subscribe(std::shared_ptr<feed::Feed> feed);

    /**
     * @brief Compare to another client by value.
     * @param other The other Client
     * @return true if equals, else false
     */
    virtual bool equals(const Client& other) const;

    /**
     * @brief Get a hash value of this client.
     * @return the hash value
     */
    virtual std::size_t hash() const;
    //< end interfaces >//
};

}  // namespace client
