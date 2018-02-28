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

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

namespace server
{
/**
 * @class Connection
 * @brief Represents a TCP connection opened by the Server.
 * @see Server.h
 */
class Connection : public boost::enable_shared_from_this<Connection>
{
public:
    /**
     * Non-copyable
     */
    Connection(const Connection&) = delete;
    /**
     * Not assignable
     */
    Connection& operator=(const Connection&) = delete;
    /**
     * @fn ~Connection
     * @brief Destructor
     */
    virtual ~Connection() noexcept;
    /**
     * @fn start
     * @brief Start a Connection.
     * @param socket The socket
     * @return a shared ptr to the Connection object
     */
    static boost::shared_ptr<Connection> start(boost::asio::ip::tcp::socket socket);
    /**
     * @fn stop
     * @brief Stop the connection, shutdown and close socket.
     */
    void stop();
    /**
     * @fn getSocket
     * @brief Get the socket as reference.
     * @return the socket ref
     */
    boost::asio::ip::tcp::socket& getSocket();
    /**
     * @fn getIp
     * @brief Get the endpoints IP address.
     * @return the IP address
     */
    const std::string& getIp();

private:
    /**
     * @fn Connection
     * @brief Constructor
     * @param socket The socket
     */
    Connection(boost::asio::ip::tcp::socket socket);
    /// Socket
    boost::asio::ip::tcp::socket mSocket;
    /// IP address
    const std::string mIp;
};

}  // namespace server
