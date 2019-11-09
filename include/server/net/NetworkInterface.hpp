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

#include "types.h"

namespace vfrb::server
{
template<typename SocketT>
class CConnection;

namespace net
{
/**
 * @brief Network interface for TCP a server.
 * @tparam SocketT The socket implementation
 */
template<typename SocketT>
class IAcceptor
{
public:
    using Callback = std::function<void(bool)>;

    IAcceptor()                   = default;
    virtual ~IAcceptor() noexcept = default;

    /**
     * @brief Run this interface.
     * @param lock The lock that may be hold and released inside
     */
    virtual void Run() = 0;

    /**
     * @brief Stop this interface.
     */
    virtual void Stop() = 0;

    /**
     * @brief Schedule an accept call.
     * @param callback The callback to invoke when done
     */
    virtual void OnAccept(Callback const& callback_) = 0;

    /**
     * @brief Close the connection.
     */
    virtual void Close() = 0;

    /**
     * @brief Start and get the current Connection.
     * @return the Connection
     */
    virtual CConnection<SocketT> StartConnection() = 0;

    /**
     * @brief Get the current connection address.
     * @return the address
     */
    virtual Str StagedAddress() const = 0;
};
}  // namespace net
}  // namespace vfrb::server
