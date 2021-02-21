/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "server/net/impl/CSocketBoost.hpp"

#include <boost/system/error_code.hpp>

#include "server/net/error/CSocketError.hpp"

using boost::asio::ip::tcp;

namespace vfrb::server::net
{
CSocketBoost::CSocketBoost(CSocketBoost&& other_) noexcept : m_socket(boost::move(other_.m_socket)) {}

auto
CSocketBoost::operator=(CSocketBoost&& other_) noexcept -> CSocketBoost& {
    m_socket = boost::move(other_.m_socket);
    return *this;
}

CSocketBoost::CSocketBoost(BOOST_RV_REF(tcp::socket) sock_) : m_socket(boost::move(sock_)) {
    if (m_socket.is_open()) {
        m_socket.non_blocking(true);
    }
}

CSocketBoost::~CSocketBoost() noexcept {
    Close();
}

auto
CSocketBoost::Address() const -> String {
    if (!m_socket.is_open()) {
        throw error::CSocketError("cannot get address from closed socket");
    }
    return m_socket.remote_endpoint().address().to_string();
}

auto
CSocketBoost::Write(String const& str_) -> bool {
    if (!m_socket.is_open()) {
        throw error::CSocketError("cannot write on closed socket");
    }
    boost::system::error_code ec;
    boost::asio::write(m_socket, boost::asio::buffer(str_), ec);
    return !ec;
}

void
CSocketBoost::Close() {
    if (m_socket.is_open()) {
        boost::system::error_code ignored_ec;
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ignored_ec);
        m_socket.close();
    }
}

auto
CSocketBoost::Get() -> tcp::socket& {
    return m_socket;
}
}  // namespace vfrb::server::net
