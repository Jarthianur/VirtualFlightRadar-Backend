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

#include "SocketImplBoost.h"

#include <boost/system/error_code.hpp>

#include "SocketException.h"

namespace server
{
SocketImplBoost::SocketImplBoost(SocketImplBoost&& other) : m_socket(boost::move(other.m_socket)) {}

SocketImplBoost& SocketImplBoost::operator=(SocketImplBoost&& other)
{
    m_socket = boost::move(other.m_socket);
    return *this;
}

SocketImplBoost::SocketImplBoost(BOOST_RV_REF(boost::asio::ip::tcp::socket) socket)
    : m_socket(boost::move(socket))
{
    if (m_socket.is_open())
    {
        m_socket.non_blocking(true);
    }
}

SocketImplBoost::~SocketImplBoost() noexcept
{
    close();
}

std::string SocketImplBoost::get_address() const
{
    if (!m_socket.is_open())
    {
        throw SocketException("cannot get address from closed socket");
    }
    return m_socket.remote_endpoint().address().to_string();
}

bool SocketImplBoost::write(const std::string& msg)
{
    if (!m_socket.is_open())
    {
        throw SocketException("cannot write on closed socket");
    }
    boost::system::error_code ec;
    boost::asio::write(m_socket, boost::asio::buffer(msg), ec);
    return !ec;
}

void SocketImplBoost::close()
{
    if (m_socket.is_open())
    {
        boost::system::error_code ignored_ec;
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ignored_ec);
        m_socket.close();
    }
}

boost::asio::ip::tcp::socket& SocketImplBoost::get()
{
    return m_socket;
}

}  // namespace server
