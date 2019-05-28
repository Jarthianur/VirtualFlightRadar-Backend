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

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

#include "net/impl/NetworkInterfaceImplBoost.h"
#include "util/Logger.hpp"
#include "util/defines.h"
#include "util/utility.hpp"

#include "Connection.hpp"
#include "parameters.h"

/// @def S_MAX_CLIENTS
/// The max amount of client to accept at once
#ifdef SERVER_MAX_CLIENTS
#    define S_MAX_CLIENTS SERVER_MAX_CLIENTS
#else
#    define S_MAX_CLIENTS 2
#endif

namespace server
{
/**
 * @brief A TCP server to serve the same reports to all clients.
 * @tparam SocketT The socket implementation
 */
template<typename SocketT>
class Server
{
public:
    NOT_COPYABLE(Server)

    Server();

    /**
     * @brief Constructor
     * @param port The port
     */
    explicit Server(std::uint16_t port);

    /**
     * @brief Server
     * @param interface The NetworkInterface to use
     */
    explicit Server(std::shared_ptr<net::NetworkInterface<SocketT>> interface);

    ~Server() noexcept;

    /**
     * @brief Run the Server.
     * @threadsafe
     */
    void run();

    /**
     * @brief Stop all connections.
     * @threadsafe
     */
    void stop();

    /**
     * @brief Write a message to all clients.
     * @param msg The msg to write
     * @threadsafe
     */
    void send(const util::CStringPack& msg);

private:
    /**
     * @brief Schedule to accept connections.
     */
    void accept();

    /**
     * @brief Check whether an ip address already exists in the Connection container.
     * @param address The ip address to check
     * @return true if the ip is already registered, else false
     */
    bool isConnected(const std::string& address);

    /**
     * @brief Handler for accepting connections.
     * @param error The error indicator
     */
    void attemptConnection(bool error) noexcept;

    /// NetworkInterface
    std::shared_ptr<net::NetworkInterface<SocketT>> m_netInterface;

    /// Connections container
    std::array<std::unique_ptr<Connection<SocketT>>, S_MAX_CLIENTS> m_connections;

    /// Number of active connections
    std::uint8_t m_activeConnections = 0;

    /// Running state
    bool m_running = false;

    /// Internal thread
    std::thread m_thread;

    mutable std::mutex m_mutex;
};

template<typename SocketT>
Server<SocketT>::Server(std::uint16_t port)
    : m_netInterface(std::make_shared<net::NetworkInterfaceImplBoost>(port))
{}

template<typename SocketT>
Server<SocketT>::Server(std::shared_ptr<net::NetworkInterface<SocketT>> interface) : m_netInterface(interface)
{}

template<typename SocketT>
Server<SocketT>::Server() : Server<SocketT>(4353)
{}

template<typename SocketT>
Server<SocketT>::~Server() noexcept
{
    stop();
}

template<typename SocketT>
void Server<SocketT>::run()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    logger.info("(Server) start server");
    m_running = true;
    m_thread  = std::thread([this]() {
        accept();
        std::unique_lock<std::mutex> lock(m_mutex);
        m_netInterface->run(lock);
        logger.debug("(Server) stopped");
    });
}

template<typename SocketT>
void Server<SocketT>::stop()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_running)
    {
        m_running = false;
        logger.info("(Server) stopping all connections ...");
        for (auto& it : m_connections)
        {
            if (it)
            {
                it.reset();
            }
        }
        m_activeConnections = 0;
        m_netInterface->stop();
        lock.unlock();
        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }
}

template<typename SocketT>
void Server<SocketT>::send(const util::CStringPack& msg)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (msg.second == 0 || m_activeConnections == 0)
    {
        return;
    }
    for (auto& it : m_connections)
    {
        if (it)
        {
            if (!it.get()->write(msg))
            {
                logger.warn("(Server) lost connection to: ", it.get()->get_address());
                it.reset();
                --m_activeConnections;
            }
        }
    }
}

template<typename SocketT>
void Server<SocketT>::accept()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_netInterface->onAccept(std::bind(&Server<SocketT>::attemptConnection, this, std::placeholders::_1));
}

template<typename SocketT>
bool Server<SocketT>::isConnected(const std::string& address)
{
    for (const auto& it : m_connections)
    {
        if (it && it.get()->get_address() == address)
        {
            return true;
        }
    }
    return false;
}

template<typename SocketT>
void Server<SocketT>::attemptConnection(bool error) noexcept
{
    if (!error)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        try
        {
            if (m_activeConnections < S_MAX_CLIENTS && !isConnected(m_netInterface->get_currentAddress()))
            {
                for (auto& it : m_connections)
                {
                    if (!it)
                    {
                        it = m_netInterface->startConnection();
                        ++m_activeConnections;
                        logger.info("(Server) connection from: ", it->get_address());
                        break;
                    }
                }
            }
            else
            {
                logger.info("(Server) refused connection to ", m_netInterface->get_currentAddress());
                m_netInterface->close();
            }
        }
        catch (const net::SocketException& e)
        {
            logger.warn("(Server) connection failed: ", e.what());
            m_netInterface->close();
        }
    }
    else
    {
        logger.warn("(Server) Could not accept connection");
    }
    accept();
}
}  // namespace server
