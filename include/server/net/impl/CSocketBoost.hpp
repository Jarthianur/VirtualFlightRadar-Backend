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

#include <boost/asio.hpp>
#include <boost/move/move.hpp>

#include "util/class_utils.hpp"

#include "types.hpp"

namespace vfrb::server::net
{
/// Socket implementation using boost
class CSocketBoost
{
    boost::asio::ip::tcp::socket m_socket;  ///< Underlying socket

public:
    MOVABLE(CSocketBoost)
    NOT_COPYABLE(CSocketBoost)

    explicit CSocketBoost(BOOST_RV_REF(boost::asio::ip::tcp::socket) sock_);
    ~CSocketBoost() noexcept;

    /// @throw vfrb::server::net::error::CSocketError
    String Address() const;

    /**
     * Write a message on the socket to the endpoint.
     * @param sv_ The message
     * @return true on success, else false
     * @throw vfrb::server::net::error::CSocketError
     */
    bool Write(StringView const& sv_);
    void Close();

    /// Get the underlying socket.
    boost::asio::ip::tcp::socket& Get();
};
}  // namespace vfrb::server::net
