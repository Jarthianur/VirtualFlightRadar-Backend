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

#include "Connection.h"

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <algorithm>

boost::shared_ptr<Connection> Connection::start(boost::asio::ip::tcp::socket socket)
{
    return boost::shared_ptr<Connection>(new Connection(std::move(socket)));
}

Connection::Connection(boost::asio::ip::tcp::socket socket)
        : mSocket(std::move(socket)),
          mIP(mSocket.remote_endpoint().address().to_string())
{
}

Connection::~Connection() noexcept
{
    stop();
}

void Connection::stop() noexcept
{
    boost::system::error_code ignored_ec;
    mSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    if (mSocket.is_open())
    {
        mSocket.close();
    }
}

boost::asio::ip::tcp::socket& Connection::getSocket()
{
    return mSocket;
}

const std::string& Connection::getIP()
{
    return mIP;
}
