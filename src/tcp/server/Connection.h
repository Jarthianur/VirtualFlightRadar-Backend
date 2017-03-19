/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

class Connection: public boost::enable_shared_from_this<Connection>
{
public:
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    virtual ~Connection() noexcept;

    /**
     * Start a Connection on given socket.
     * Return shared ptr to it.
     */
    static boost::shared_ptr<Connection> start(boost::asio::ip::tcp::socket /*socket*/);

    /**
     * Stop connection, shutdown and close socket.
     */
    void stop() noexcept;

    boost::asio::ip::tcp::socket& getSocket();
    const std::string& getIP();

private:
    Connection(boost::asio::ip::tcp::socket /*socket*/);
    boost::asio::ip::tcp::socket mSocket;
    const std::string mIP;
};

#endif /* SRC_TCP_SERVER_CONNECTION_H_ */
