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

#include "server/net/impl/CSocketAsio.hpp"

#include "server/net/error/CSocketError.hpp"

using asio::ip::tcp;

namespace vfrb::server::net
{
CSocketAsio::CSocketAsio(CSocketAsio&& other_) noexcept : m_socket(std::move(other_.m_socket)) {}

auto
CSocketAsio::operator=(CSocketAsio&& other_) noexcept -> CSocketAsio& {
    m_socket = std::move(other_.m_socket);
    return *this;
}

CSocketAsio::CSocketAsio(tcp::socket&& sock_) : m_socket(std::move(sock_)) {
    if (m_socket.is_open()) {
        m_socket.non_blocking(true);
    }
}

CSocketAsio::~CSocketAsio() noexcept {
    Close();
}

auto
CSocketAsio::Address() const -> String {
    if (!m_socket.is_open()) {
        throw error::CSocketError("cannot get address from closed socket");
    }
    return m_socket.remote_endpoint().address().to_string();
}

auto
CSocketAsio::Write(String const& str_) -> bool {
    if (!m_socket.is_open()) {
        throw error::CSocketError("cannot write on closed socket");
    }
    asio::error_code ec;
    asio::write(m_socket, asio::buffer(str_), ec);
    return !ec;
}

void
CSocketAsio::Close() {
    if (m_socket.is_open()) {
        asio::error_code ignored_ec;
        m_socket.shutdown(asio::ip::tcp::socket::shutdown_send, ignored_ec);
        m_socket.close();
    }
}

auto
CSocketAsio::Get() -> tcp::socket& {
    return m_socket;
}
}  // namespace vfrb::server::net
