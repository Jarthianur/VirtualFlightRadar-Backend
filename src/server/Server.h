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
#include <string>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "../Defines.h"
#include "../Parameters.h"
#include "Connection.h"

#ifdef SERVER_MAX_CLIENTS
/// @def S_MAX_CLIENTS
/// The max amount of client to accept at once
#define S_MAX_CLIENTS SERVER_MAX_CLIENTS
#else
#define S_MAX_CLIENTS 5
#endif

/// @namespace server
namespace server
{
/**
 * @class Server
 * @brief A TCP server to serve the same reports to all clients.
 */
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
    explicit Server(std::uint16_t vPort);

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
    void send(const std::string& crStr);

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
    bool isConnected(const std::string& crIpAddress);

    /**
     * @fn handleAccept
     * @brief Handler for accept
     * @param crError The error code
     * @threadsafe
     */
    void handleAccept(const boost::system::error_code& crError) noexcept;

    void attemptConnection() noexcept;

    boost::thread mThread;

    /// @var mMutex
    /// Mutex
    boost::mutex mMutex;

    /// @var mIoService
    /// Internal IO-service
    boost::asio::io_service mIoService;

    /// @var mAcceptor
    /// Acceptor
    boost::asio::ip::tcp::acceptor mAcceptor;

    /// @var mSocket
    /// Socket
    boost::asio::ip::tcp::socket mSocket;

    /// @var mConnections
    /// Vector holding Connections
    std::array<std::unique_ptr<Connection>, S_MAX_CLIENTS> mConnections;

    std::size_t mActiveConnections = 0;
};

}  // namespace server
