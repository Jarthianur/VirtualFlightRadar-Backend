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

#pragma once

#include "util/ClassUtils.hpp"

#include "Types.hpp"
#include "asio.hpp"

namespace vfrb::server::net
{
/// Socket implementation using boost
class CSocketAsio
{
    asio::ip::tcp::socket m_socket;  ///< Underlying socket

public:
    MOVABLE(CSocketAsio)
    NOT_COPYABLE(CSocketAsio)

    explicit CSocketAsio(asio::ip::tcp::socket&& sock_);
    ~CSocketAsio() noexcept;

    /// @throw vfrb::server::net::error::CSocketError
    [[nodiscard]] auto
    Address() const -> String;

    /**
     * Write a message on the socket to the endpoint.
     * @param sv_ The message
     * @return true on success, else false
     * @throw vfrb::server::net::error::CSocketError
     */
    auto
    Write(String const& str_) -> bool;

    void
    Close();

    /// Get the underlying socket.
    auto
    Get() -> asio::ip::tcp::socket&;
};
}  // namespace vfrb::server::net
