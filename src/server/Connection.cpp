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

#include "Connection.h"

#include <algorithm>
#include <boost/move/move.hpp>
#include <boost/system/error_code.hpp>

namespace server
{
boost::shared_ptr<Connection> Connection::start(BOOST_RV_REF(boost::asio::ip::tcp::socket) rvSocket)
{
    return boost::shared_ptr<Connection>(new Connection(boost::move(rvSocket)));
}

Connection::Connection(BOOST_RV_REF(boost::asio::ip::tcp::socket) rvSocket)
    : mSocket(boost::move(rvSocket)), mIpAddress(mSocket.remote_endpoint().address().to_string())
{}

Connection::~Connection() noexcept
{
    stop();
}

void Connection::stop()
{
    boost::system::error_code ignored_ec;
    mSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    if(mSocket.is_open())
    {
        mSocket.close();
    }
}

boost::asio::ip::tcp::socket& Connection::getSocket()
{
    return mSocket;
}

}  // namespace server
