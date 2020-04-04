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

#include "server/net/impl/CSocketBoost.hpp"

#include <boost/system/error_code.hpp>

#include "server/net/error/CSocketError.hpp"

namespace vfrb::server::net
{
CSocketBoost::CSocketBoost(CSocketBoost&& other_) noexcept : m_socket(boost::move(other_.m_socket)) {}

CSocketBoost& CSocketBoost::operator=(CSocketBoost&& other_) noexcept {
    m_socket = boost::move(other_.m_socket);
    return *this;
}

CSocketBoost::CSocketBoost(BOOST_RV_REF(boost::asio::ip::tcp::socket) sock_) : m_socket(boost::move(sock_)) {
    if (m_socket.is_open()) {
        m_socket.non_blocking(true);
    }
}

CSocketBoost::~CSocketBoost() noexcept {
    Close();
}

String CSocketBoost::Address() const {
    if (!m_socket.is_open()) {
        throw error::CSocketError("cannot get address from closed socket");
    }
    return m_socket.remote_endpoint().address().to_string();
}

bool CSocketBoost::Write(StringView const& sv_) {
    if (!m_socket.is_open()) {
        throw error::CSocketError("cannot write on closed socket");
    }
    boost::system::error_code ec;
    boost::asio::write(m_socket, boost::asio::buffer(sv_.data(), sv_.length()), ec);
    return !ec;
}

void CSocketBoost::Close() {
    if (m_socket.is_open()) {
        boost::system::error_code ignored_ec;
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ignored_ec);
        m_socket.close();
    }
}

boost::asio::ip::tcp::socket& CSocketBoost::Get() {
    return m_socket;
}
}  // namespace vfrb::server::net
