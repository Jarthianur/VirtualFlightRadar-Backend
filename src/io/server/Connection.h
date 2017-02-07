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

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

class Connection: public boost::enable_shared_from_this<Connection>
{
public:
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    virtual ~Connection() throw ();

    static boost::shared_ptr<Connection> start(boost::asio::ip::tcp::socket socket);
    void stop();
    boost::asio::ip::tcp::socket& socket();
    const std::string& ip();

private:
    Connection(boost::asio::ip::tcp::socket socket);
    boost::asio::ip::tcp::socket socket_;
    const std::string ip_;
};

#endif /* CONNECTION_H_ */
