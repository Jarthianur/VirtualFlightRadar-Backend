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

#include <memory>
#include <string>
#include <utility>

#include "net/SocketException.h"
#include "util/Logger.hpp"
#include "util/defines.h"

namespace server
{
/**
 * @brief TCP connection opened by the Server.
 * @tparam SocketT The type of socket implementation
 */
template<typename SocketT>
class Connection final
{
public:
    NOT_COPYABLE(Connection)
    DEFAULT_DTOR(Connection)

    /**
     * @brief Start a Connection.
     * @param socket The socket
     * @return a shared ptr to the Connection object
     */
    static std::unique_ptr<Connection<SocketT>> create(SocketT&& socket);

    /**
     * @brief Write a message to the endpoint.
     * @param msg The message
     * @return true on success, else false
     */
    bool write(const std::string& msg);

private:
    /**
     * @brief Constructor
     * @param socket The socket
     */
    explicit Connection(SocketT&& socket);

    /// Socket
    SocketT m_socket;

    /// IP address
    const std::string m_address;

public:
    /**
     * Getters
     */
    GETTER_CR(address)
};

template<typename SocketT>
std::unique_ptr<Connection<SocketT>> Connection<SocketT>::create(SocketT&& socket)
{
    return std::unique_ptr<Connection<SocketT>>(new Connection<SocketT>(std::move(socket)));
}

template<typename SocketT>
bool Connection<SocketT>::write(const std::string& msg)
{
    try
    {
        return m_socket.write(msg);
    }
    catch (const net::SocketException& e)
    {
        logger.debug("(Connection) write: ", e.what());
    }
    return false;
}

template<typename SocketT>
Connection<SocketT>::Connection(SocketT&& socket)
    : m_socket(std::move(socket)), m_address(m_socket.get_address())
{}

}  // namespace server
