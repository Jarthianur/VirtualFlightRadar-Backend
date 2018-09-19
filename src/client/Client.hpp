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
#include <utility>
#include <vector>
#include <boost/functional/hash.hpp>

#include "../Defines.h"
#include "../Logger.hpp"
#include "../Parameters.h"
#include "../feed/Feed.h"
#include "Endpoint.hpp"

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
template<typename ConnectorT>
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

    void lockAndStop();

    virtual bool equals(const Client<ConnectorT>& other) const;

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
    Client(const Endpoint& endpoint, const std::string& component);

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
    void handleTimedConnect(bool error) noexcept;

    /**
     * @fn handleRead
     * @brief Handler for read.
     * @param crError The error code
     * @param vBytes  The sent bytes
     */
    void handleRead(bool error, const std::string& response) noexcept;

    /**
     * @fn handleConnect
     * @brief Handler for connect.
     * @param crError     The error code
     * @param vResolverIt The resolve iterator
     */
    virtual void handleConnect(bool error) noexcept = 0;

    ConnectorT m_connector;

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

template<typename ConnectorT>
Client<ConnectorT>::Client(const Endpoint& endpoint, const std::string& component)
    : m_component(component), m_endpoint(endpoint)
{}

template<typename ConnectorT>
Client<ConnectorT>::~Client() noexcept
{
    stop();
}

template<typename ConnectorT>
void Client<ConnectorT>::run()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if(!m_running)
    {
        m_running = true;
        connect();
        lock.unlock();
        m_connector.run();
    }
}

template<typename ConnectorT>
bool Client<ConnectorT>::equals(const Client& other) const
{
    return this->m_endpoint == other.m_endpoint;
}

template<typename ConnectorT>
std::size_t Client<ConnectorT>::hash() const
{
    std::size_t seed = 0;
    boost::hash_combine(seed, boost::hash_value(m_endpoint.host));
    boost::hash_combine(seed, boost::hash_value(m_endpoint.port));
    return seed;
}

template<typename ConnectorT>
void Client<ConnectorT>::subscribe(std::shared_ptr<feed::Feed> feed)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_feeds.push_back(feed);
}

template<typename ConnectorT>
void Client<ConnectorT>::connect()
{
    m_connector.onConnect(m_endpoint,
                          std::bind(&Client::handleConnect, this, std::placeholders::_1));
}

template<typename ConnectorT>
void Client<ConnectorT>::reconnect()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_running)
    {
        logger.info(m_component, " schedule reconnect to ", m_endpoint.host, ":", m_endpoint.port);
        m_connector.close();
        timedConnect();
    }
}

template<typename ConnectorT>
void Client<ConnectorT>::timedConnect()
{
    m_connector.onTimeout(std::bind(&Client::handleTimedConnect, this, std::placeholders::_1),
                          C_CON_WAIT_TIMEVAL);
}

template<typename ConnectorT>
void Client<ConnectorT>::stop()
{
    if(m_running)
    {
        m_running = false;
        logger.info(m_component, " disconnect from ", m_endpoint.host, ":", m_endpoint.port);
        m_connector.stop();
    }
}

template<typename ConnectorT>
void Client<ConnectorT>::lockAndStop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    stop();
}

template<typename ConnectorT>
void Client<ConnectorT>::read()
{
    m_connector.onRead(
        std::bind(&Client::handleRead, this, std::placeholders::_1, std::placeholders::_2));
}

template<typename ConnectorT>
void Client<ConnectorT>::handleTimedConnect(bool error) noexcept
{
    if(!error)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        logger.info(m_component, " try connect to ", m_endpoint.host, ":", m_endpoint.port);
        connect();
    }
    else
    {
        logger.error(m_component, " failed to connect after timeout");
        lockAndStop();
    }
}

template<typename ConnectorT>
void Client<ConnectorT>::handleRead(bool error, const std::string& response) noexcept
{
    if(!error)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for(auto& it : m_feeds)
        {
            if(!it->process(response))
            {
                stop();
            }
        }
        read();
    }
    else
    {
        logger.error(m_component, " failed to read message");
        reconnect();
    }
}

}  // namespace client
