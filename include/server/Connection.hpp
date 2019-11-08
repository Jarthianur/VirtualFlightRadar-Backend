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

#include <string_view>
#include <utility>

#include "net/error/SocketError.h"
#include "util/Logger.hpp"
#include "util/class_utils.h"

#include "types.h"

namespace vfrb::server
{
/**
 * @brief TCP connection opened by the Server.
 * @tparam SocketT The type of socket implementation
 */
template<typename SocketT>
class Connection
{
    SocketT m_socket;   ///< Socket
    Str     m_address;  ///< IP address

public:
    MOVABLE_BUT_NOT_COPYABLE(Connection)

    /**
     * @brief Constructor
     * @param socket The socket
     */
    explicit Connection(SocketT&& socket);

    ~Connection() noexcept = default;

    /**
     * @brief Write a message to the endpoint.
     * @param msg The message
     * @return true on success, else false
     */
    bool write(std::string_view const& msg);

    auto address() const -> decltype(m_address) const&;
};

template<typename SocketT>
Connection<SocketT>::Connection(SocketT&& socket) : m_socket(std::move(socket)), m_address(m_socket.address())
{}

template<typename SocketT>
Connection<SocketT>::Connection(Connection&& other)
    : m_socket(std::move(other.m_socket)), m_address(std::move(other.m_address))
{}

template<typename SocketT>
Connection<SocketT>& Connection<SocketT>::operator=(Connection&& other)
{
    m_socket  = std::move(other.m_socket);
    m_address = std::move(other.m_address);
    return *this;
}

template<typename SocketT>
bool Connection<SocketT>::write(std::string_view const& msg)
{
    try
    {
        return m_socket.write(msg);
    }
    catch (net::error::SocketError const& e)
    {
        CLogger::Instance().debug("(Connection) write: ", e.what());
    }
    return false;
}

template<typename SocketT>
auto Connection<SocketT>::address() const -> decltype(m_address) const&
{
    return m_address;
}
}  // namespace vfrb::server
