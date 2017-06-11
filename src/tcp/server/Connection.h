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

#ifndef SRC_TCP_SERVER_CONNECTION_H_
#define SRC_TCP_SERVER_CONNECTION_H_

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

namespace tcp
{
namespace server
{

/**
 * The Connection class.
 *
 * This class represents a TCP connection opened by the Server.
 * The connection is closed with destruction.
 * May only be used as shared pointer to a singleton Connection.
 */
class Connection: public boost::enable_shared_from_this<Connection>
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
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~Connection() noexcept;
    /**
     * Start a Connection on given socket.
     * Return shared ptr to it.
     *
     * @param socket the socket
     *
     * @return shared ptr to the Connection object
     */
    static boost::shared_ptr<Connection> start(
            boost::asio::ip::tcp::socket /*socket*/);
    /**
     * Stop connection, shutdown and close socket.
     *
     * @exceptsafe strong
     */
    void stop() noexcept;
    /**
     * Get the socket as reference.
     *
     * @return the socket ref
     */
    boost::asio::ip::tcp::socket& getSocket();
    /**
     * Get the endpoints IP address.
     *
     * @return the IP address
     */
    const std::string& getIP();

private:
    /**
     * Constructor
     *
     * @param socket the socket
     */
    Connection(boost::asio::ip::tcp::socket /*socket*/);
    /// Socket
    boost::asio::ip::tcp::socket mSocket;
    /// IP address
    const std::string mIP;
};

}  // namespace server
}  // namespace tcp

#endif /* SRC_TCP_SERVER_CONNECTION_H_ */
