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

#include "../Defines.h"
#include "../Parameters.h"
#include "Connector.hpp"
#include "Endpoint.hpp"

namespace feed
{
class Feed;
}  // namespace feed
namespace client
{
/// @def C_CON_WAIT_TIMEVAL
/// Wait for (re-)connect timeout
#ifdef CLIENT_CONNECT_WAIT_TIMEVAL
#define C_CON_WAIT_TIMEVAL CLIENT_CONNECT_WAIT_TIMEVAL
#else
#define C_CON_WAIT_TIMEVAL 120
#endif

/**
 * @brief Base class representing a TCP client.
 * @tparam ConnectorT The connector interface
 */
class Client
{
public:
    NOT_COPYABLE(Client)

    /**
     * @brief Destructor
     */
    virtual ~Client() noexcept;

    /**
     * @brief Run the Client.
     * @note Returns after all queued handlers have returned.
     */
    void run();

    void scheduleStop();

    virtual bool equals(const Client& other) const;

    virtual std::size_t hash() const;

    void subscribe(std::shared_ptr<feed::Feed> feed);

protected:
    /**
     * @fn Client
     * @brief Constructor
     * @param crHost       The hostname
     * @param crPort       The port
     * @param crComponent  The component name
     * @param rFeed        The handler Feed reference
     */
    Client(const Endpoint& endpoint, const std::string& component,
           std::shared_ptr<Connector> connector);

    /**
     * @fn stop
     * @brief Stop the Client and close the connection.
     */
    virtual void stop();

    /**
     * @fn timedConnect
     * @brief Connect after timeout.
     */
    void timedConnect();

    /**
     * @fn read
     * @brief Read from remote endpoint.
     */
    virtual void read();

    /**
     * @fn connect
     * @brief Connect to host.
     */
    void connect();

    void reconnect();

    /**
     * @fn handleTimedConnect
     * @brief Handler for timedConnect.
     * @param crError The error code
     */
    void handleTimedConnect(bool error);

    /**
     * @fn handleRead
     * @brief Handler for read.
     * @param crError The error code
     * @param vBytes  The sent bytes
     */
    void handleRead(bool error, const std::string& response);

    /**
     * @fn handleConnect
     * @brief Handler for connect.
     * @param crError     The error code
     * @param vResolverIt The resolve iterator
     */
    virtual void handleConnect(bool error) = 0;

    std::shared_ptr<Connector> m_connector;

    bool m_running = false;

    /// @var mComponent
    /// Component string used for logging
    const std::string m_component;

    /// @var mHost
    /// Hostname
    const Endpoint m_endpoint;

    mutable std::mutex m_mutex;

private:
    /// @var mrFeeds
    /// Handler Feed references
    std::vector<std::shared_ptr<feed::Feed>> m_feeds;
};

}  // namespace client
