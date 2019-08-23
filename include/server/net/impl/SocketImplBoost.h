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

#pragma once

#include <boost/asio.hpp>
#include <boost/move/move.hpp>

#include "util/defines.h"
#include "util/types.h"
#include "util/utility.hpp"

namespace server::net
{
/**
 * @brief Socket implementation using boost
 */
class SocketImplBoost
{
    boost::asio::ip::tcp::socket m_socket;  ///< Underlying socket

public:
    MOVABLE_BUT_NOT_COPYABLE(SocketImplBoost)

    explicit SocketImplBoost(BOOST_RV_REF(boost::asio::ip::tcp::socket)
                                 socket);  ///< @param socket The underlying socket
    ~SocketImplBoost() noexcept;

    /**
     * @brief Get the remote IP address.
     * @return the address
     * @throw SocketException if the socket is closed
     */
    str address() const;

    /**
     * @brief Write a message on the socket to the endpoint.
     * @param msg The message
     * @return true on success, else false
     * @throw SocketException if the socket is closed
     */
    bool write(util::CStringPack const& msg);

    /**
     * @brief Close the socket.
     */
    void close();

    /**
     * @brief Get the underlying socket.
     * @return the socket
     */
    boost::asio::ip::tcp::socket& get();
};
}  // namespace server::net
