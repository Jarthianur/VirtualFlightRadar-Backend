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

#include "../Defines.h"
#include "../Logger.hpp"
#include "SocketException.h"

/// @namespace server
namespace server
{
/**
 * @class Connection
 * @brief TCP connection opened by the Server.
 * @see Server.h
 */
template<typename SocketT>
class Connection final
{
public:
    NOT_COPYABLE(Connection)

    /**
     * @fn ~Connection
     * @brief Destructor
     */
    ~Connection() noexcept;

    /**
     * @fn start
     * @brief Start a Connection.
     * @param rvSocket The socket
     * @return a shared ptr to the Connection object
     */
    static std::unique_ptr<Connection<SocketT>> start(SocketT&& rvSocket);

    bool write(const std::string& crStr);

private:
    /**
     * @fn Connection
     * @brief Constructor
     * @param rvSocket The socket
     */
    explicit Connection(SocketT&& rvSocket);

    /// @var mSocket
    /// Socket
    SocketT m_socket;

    /// @var mIpAddress
    /// IP address
    const std::string m_ipAddress;

public:
    /**
     * Define and declare getters.
     */
    GETTER_CR(ipAddress)
};

template<typename SocketT>
Connection<SocketT>::~Connection<SocketT>() noexcept
{}

template<typename SocketT>
std::unique_ptr<Connection<SocketT>> Connection<SocketT>::start(SocketT&& rvSocket)
{
    return std::unique_ptr<Connection<SocketT>>(new Connection<SocketT>(std::move(rvSocket)));
}

template<typename SocketT>
bool Connection<SocketT>::write(const std::string& crStr)
{
    try
    {
        return m_socket.write(crStr);
    }
    catch(const SocketException& crSE)
    {
        logger.debug("(Connection) write: ", crSE.what());
    }
    return false;
}

template<typename SocketT>
Connection<SocketT>::Connection(SocketT&& rvSocket)
    : m_socket(std::move(rvSocket)), m_ipAddress(m_socket.address())
{}

}  // namespace server
