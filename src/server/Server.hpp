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
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

#include "../Defines.h"
#include "../Logger.hpp"
#include "../Parameters.h"
#include "Connection.hpp"

#ifdef SERVER_MAX_CLIENTS
/// @def S_MAX_CLIENTS
/// The max amount of client to accept at once
#define S_MAX_CLIENTS SERVER_MAX_CLIENTS
#else
#define S_MAX_CLIENTS 2
#endif

/// @namespace server
namespace server
{
/**
 * @class Server
 * @brief A TCP server to serve the same reports to all clients.
 */
template<typename TcpInterfaceT, typename SocketT>
class Server
{
public:
    NOT_COPYABLE(Server)

    Server();

    /**
     * @fn Server
     * @brief Constructor
     * @param vPort The port
     */
    explicit Server(std::uint16_t port);

    ~Server() noexcept;

    /**
     * @fn run
     * @brief Run the Server.
     * @note Returns after all operations in the queue have returned.
     */
    void run();

    /**
     * @fn stop
     * @brief Stop all connections.
     * @threadsafe
     */
    void stop();

    /**
     * @fn send
     * @brief Write a message to all clients.
     * @param crStr The msg to write
     * @threadsafe
     */
    void send(const std::string& msg);

private:
    /**
     * @fn accept
     * @brief Accept connections.
     */
    void accept();

    /**
     * @fn isConnected
     * @brief Check whether an ip address already exists in the Connection container.
     * @param crIpAddress The ip address to check
     * @return true if the ip is already registered, else false
     */
    bool isConnected(const std::string& address);

    void attemptConnection(bool error) noexcept;

    std::thread m_thread;

    /// @var mMutex
    /// Mutex
    mutable std::mutex m_mutex;

    TcpInterfaceT m_tcpIf;

    /// @var mConnections
    /// Vector holding Connections
    std::array<std::unique_ptr<Connection<SocketT>>, S_MAX_CLIENTS> m_connections;

    std::uint32_t m_activeConnections = 0;

    bool m_running = false;
};

template<typename TcpInterfaceT, typename SocketT>
Server<TcpInterfaceT, SocketT>::Server(std::uint16_t port) : m_tcpIf(port)
{}

template<typename TcpInterfaceT, typename SocketT>
Server<TcpInterfaceT, SocketT>::Server() : Server<TcpInterfaceT, SocketT>(4353)
{}

template<typename TcpInterfaceT, typename SocketT>
Server<TcpInterfaceT, SocketT>::~Server() noexcept
{
    stop();
}

template<typename TcpInterfaceT, typename SocketT>
void Server<TcpInterfaceT, SocketT>::run()
{
    logger.info("(Server) start server");
    std::lock_guard<std::mutex> lock(m_mutex);
    m_running = true;
    m_thread  = std::thread([this]() {
        accept();
        std::unique_lock<std::mutex> lock(m_mutex);
        m_tcpIf.run(lock);
        logger.debug("(Server) stopped");
    });
}

template<typename TcpInterfaceT, typename SocketT>
void Server<TcpInterfaceT, SocketT>::stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_running)
    {
        m_running = false;
        logger.info("(Server) stopping all connections ...");
        for(auto& it : m_connections)
        {
            if(it)
            {
                it.reset();
            }
        }
        m_activeConnections = 0;
        m_tcpIf.stop();
        if(m_thread.joinable())
        {
            m_thread.join();
        }
    }
}

template<typename TcpInterfaceT, typename SocketT>
void Server<TcpInterfaceT, SocketT>::send(const std::string& msg)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(msg.empty() || m_activeConnections == 0)
    {
        return;
    }
    for(auto& it : m_connections)
    {
        if(it)
        {
            if(!it.get()->write(msg))
            {
                logger.warn("(Server) lost connection to: ", it.get()->get_address());
                it.reset();
                --m_activeConnections;
            }
        }
    }
}

template<typename TcpInterfaceT, typename SocketT>
void Server<TcpInterfaceT, SocketT>::accept()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_tcpIf.onAccept(
        std::bind(&Server<TcpInterfaceT, SocketT>::attemptConnection, this, std::placeholders::_1));
}

template<typename TcpInterfaceT, typename SocketT>
bool Server<TcpInterfaceT, SocketT>::isConnected(const std::string& address)
{
    for(const auto& it : m_connections)
    {
        if(it && it.get()->get_address() == address)
        {
            return true;
        }
    }
    return false;
}

template<typename TcpInterfaceT, typename SocketT>
void Server<TcpInterfaceT, SocketT>::attemptConnection(bool error) noexcept
{
    if(!error)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        try
        {
            if(m_activeConnections < S_MAX_CLIENTS && !isConnected(m_tcpIf.get_socket().get_address()))
            {
                for(auto& it : m_connections)
                {
                    if(!it)
                    {
                        it = Connection<SocketT>::start(std::move(m_tcpIf.get_socket()));
                        ++m_activeConnections;
                        logger.info("(Server) connection from: ", it->get_address());
                        break;
                    }
                }
            }
            else
            {
                logger.info("(Server) refused connection to ", m_tcpIf.get_socket().get_address());
                m_tcpIf.close();
            }
        }
        catch(const SocketException& e)
        {
            logger.warn("(Server) connection failed: ", e.what());
            m_tcpIf.close();
        }
    }
    else
    {
        logger.warn("(Server) Could not accept connection");
    }
    accept();
}

}  // namespace server
