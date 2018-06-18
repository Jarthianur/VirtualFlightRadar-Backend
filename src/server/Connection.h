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

#include <string>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/move/move.hpp>
#include <boost/shared_ptr.hpp>

#include "../Defines.h"

/// @namespace server
namespace server
{
/**
 * @class Connection
 * @brief TCP connection opened by the Server.
 * @see Server.h
 */
class Connection : public boost::enable_shared_from_this<Connection>
{
public:
    NON_COPYABLE(Connection)

    /**
     * @fn ~Connection
     * @brief Destructor
     */
    virtual ~Connection() noexcept;

    /**
     * @fn start
     * @brief Start a Connection.
     * @param rvSocket The socket
     * @return a shared ptr to the Connection object
     */
    static boost::shared_ptr<Connection> start(BOOST_RV_REF(boost::asio::ip::tcp::socket) rvSocket);

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
     * Define and declare getters.
     */
    GETTER_CR(std::string, mIpAddress, IpAddress)

private:
    /**
     * @fn Connection
     * @brief Constructor
     * @param rvSocket The socket
     */
    explicit Connection(BOOST_RV_REF(boost::asio::ip::tcp::socket) rvSocket);

    /// @var mSocket
    /// Socket
    boost::asio::ip::tcp::socket mSocket;

    /// @var mIpAddress
    /// IP address
    const std::string mIpAddress;
};

}  // namespace server
