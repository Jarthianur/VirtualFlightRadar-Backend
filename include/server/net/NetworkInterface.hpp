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

#include <functional>
#include <mutex>

#include "types.h"

namespace vfrb::server
{
template<typename SocketT>
class Connection;

namespace net
{
/**
 * @brief Network interface for TCP a server.
 * @tparam SocketT The socket implementation
 */
template<typename SocketT>
class NetworkInterface
{
public:
    using Callback = std::function<void(bool)>;

    NetworkInterface()                   = default;
    virtual ~NetworkInterface() noexcept = default;

    /**
     * @brief Run this interface.
     * @param lock The lock that may be hold and released inside
     */
    virtual void run(std::unique_lock<std::mutex>& lk) = 0;

    /**
     * @brief Stop this interface.
     */
    virtual void stop() = 0;

    /**
     * @brief Schedule an accept call.
     * @param callback The callback to invoke when done
     */
    virtual void onAccept(Callback const& callback) = 0;

    /**
     * @brief Close the connection.
     */
    virtual void close() = 0;

    /**
     * @brief Start and get the current Connection.
     * @return the Connection
     */
    virtual u_ptr<Connection<SocketT>> startConnection() = 0;

    /**
     * @brief Get the current connection address.
     * @return the address
     */
    virtual str stagedAddress() const = 0;
};
}  // namespace net
}  // namespace vfrb::server
