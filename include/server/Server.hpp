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

#include <algorithm>
#include <string_view>
#include <vector>

#include "concurrent/GuardedThread.hpp"
#include "concurrent/Mutex.hpp"
#include "net/impl/NetworkInterfaceImplBoost.h"
#include "util/Logger.hpp"
#include "util/class_utils.h"

#include "Connection.hpp"
#include "types.h"

namespace vfrb::server
{
/**
 * @brief A TCP server to serve the same reports to all clients.
 * @tparam SocketT The socket implementation
 */
template<typename SocketT>
class Server
{
    NOT_COPYABLE(Server)

    static char const*   LOG_PREFIX;
    static Logger const& logger;

    concurrent::Mutex mutable m_mutex;
    s_ptr<net::NetworkInterface<SocketT>> GUARDED_BY(m_mutex) m_netInterface;  ///< NetworkInterface
    std::vector<Connection<SocketT>>      GUARDED_BY(m_mutex) m_connections;   ///< Connections container
    usize const                           GUARDED_BY(m_mutex) m_maxConnections;
    bool                                  GUARDED_BY(m_mutex) m_running = false;  ///< Running state
    concurrent::GuardedThread             GUARDED_BY(m_mutex) m_thread;           ///< Internal thread

    /**
     * @brief Schedule to accept connections.
     */
    void accept() REQUIRES(!m_mutex);

    /**
     * @brief Check whether an ip address already exists in the Connection container.
     * @param address The ip address to check
     * @return true if the ip is already registered, else false
     */
    bool isConnected(str const& address) REQUIRES(m_mutex);

    /**
     * @brief Handler for accepting connections.
     * @param error The error indicator
     */
    void attemptConnection(bool error) noexcept REQUIRES(!m_mutex);

public:
    Server(u16 port, usize maxCon);  ///< @param port The port
    Server(s_ptr<net::NetworkInterface<SocketT>> interface,
           usize                                 maxCon);  ///< @param interface The NetworkInterface to use
    ~Server() noexcept;

    /**
     * @brief Run the Server.
     * @threadsafe
     */
    void run() REQUIRES(!m_mutex);

    /**
     * @brief Stop all connections.
     * @threadsafe
     */
    void stop() REQUIRES(!m_mutex);

    /**
     * @brief Write a message to all clients.
     * @param msg The msg to write
     * @threadsafe
     */
    void send(std::string_view const& msg) REQUIRES(!m_mutex);
};

template<typename SocketT>
char const* Server<SocketT>::LOG_PREFIX = "(Server) ";

template<typename SocketT>
Logger const& Server<SocketT>::logger = Logger::instance();

template<typename SocketT>
Server<SocketT>::Server(u16 port, usize maxCon)
    : Server(std::make_shared<net::NetworkInterfaceImplBoost>(port), maxCon)
{}

template<typename SocketT>
Server<SocketT>::Server(s_ptr<net::NetworkInterface<SocketT>> interface, usize maxCon)
    : m_netInterface(interface), m_maxConnections(maxCon)
{
    m_connections.reserve(maxCon);
}

template<typename SocketT>
Server<SocketT>::~Server() noexcept
{
    stop();
}

template<typename SocketT>
void Server<SocketT>::run() REQUIRES(!m_mutex)
{
    concurrent::LockGuard lk(m_mutex);
    logger.info(LOG_PREFIX, "starting...");
    m_running = true;
    m_thread.spawn([this]() {
        accept();
        m_netInterface->run();
        logger.debug(LOG_PREFIX, "stopped");
    });
}

template<typename SocketT>
void Server<SocketT>::stop() REQUIRES(!m_mutex)
{
    concurrent::LockGuard lk(m_mutex);
    if (m_running)
    {
        m_running = false;
        logger.info(LOG_PREFIX, "stopping all connections...");
        m_connections.clear();
        m_netInterface->stop();
    }
}

template<typename SocketT>
void Server<SocketT>::send(std::string_view const& msg) REQUIRES(!m_mutex)
{
    concurrent::LockGuard lk(m_mutex);
    if (msg.empty() || m_connections.empty())
    {
        return;
    }
    for (auto it = m_connections.begin(); it != m_connections.end();)
    {
        if (!it->write(msg))
        {
            logger.warn(LOG_PREFIX, "lost connection to: ", it->address());
            it = m_connections.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

template<typename SocketT>
void Server<SocketT>::accept() REQUIRES(!m_mutex)
{
    concurrent::LockGuard lk(m_mutex);
    m_netInterface->onAccept(std::bind(&Server<SocketT>::attemptConnection, this, std::placeholders::_1));
}

template<typename SocketT>
bool Server<SocketT>::isConnected(str const& address) REQUIRES(m_mutex)
{
    return std::find_if(m_connections.begin(), m_connections.end(),
                        [&](auto const& it) { return it.address() == address; }) != m_connections.end();
}

template<typename SocketT>
void Server<SocketT>::attemptConnection(bool error) noexcept REQUIRES(!m_mutex)
{
    if (!error)
    {
        concurrent::LockGuard lk(m_mutex);
        try
        {
            if (m_connections.size() < m_maxConnections && !isConnected(m_netInterface->stagedAddress()))
            {
                m_connections.push_back(m_netInterface->startConnection());
                logger.info(LOG_PREFIX, "connection from: ", m_connections.crbegin()->address());
            }
            else
            {
                logger.info(LOG_PREFIX, "refused connection to ", m_netInterface->stagedAddress());
                m_netInterface->close();
            }
        }
        catch (net::error::SocketError const& e)
        {
            logger.warn(LOG_PREFIX, "connection failed: ", e.what());
            m_netInterface->close();
        }
    }
    else
    {
        logger.warn(LOG_PREFIX, "could not accept connection");
    }
    accept();
}
}  // namespace vfrb::server
