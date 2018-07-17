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

#include <boost/system/error_code.hpp>

#include "SocketImplBoost.h"

namespace server
{
SocketImplBoost::SocketImplBoost(SocketImplBoost&& rvOther) : mSocket(boost::move(rvOther.mSocket))
{}

SocketImplBoost& SocketImplBoost::operator=(SocketImplBoost&& rvOther)
{
    mSocket = boost::move(rvOther.mSocket);
    return *this;
}

SocketImplBoost::SocketImplBoost(BOOST_RV_REF(boost::asio::ip::tcp::socket) rvSocket)
    : mSocket(boost::move(rvSocket))
{
    if(mSocket.is_open())
    {
        mSocket.non_blocking(true);
    }
}

SocketImplBoost::~SocketImplBoost() noexcept
{
    close();
}

std::string SocketImplBoost::address() const
{
    return mSocket.remote_endpoint().address().to_string();
}

bool SocketImplBoost::write(const std::string& crStr)
{
    boost::system::error_code ec;
    boost::asio::write(mSocket, boost::asio::buffer(crStr), ec);
    return !ec;
}

void SocketImplBoost::close()
{
    if(mSocket.is_open())
    {
        boost::system::error_code ignored_ec;
        mSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ignored_ec);
        mSocket.close();
    }
}

boost::asio::ip::tcp::socket& SocketImplBoost::get()
{
    return mSocket;
}

}  // namespace server
