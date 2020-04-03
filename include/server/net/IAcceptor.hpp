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

#include "util/class_utils.hpp"

#include "types.hpp"

namespace vfrb::server
{
template<typename SocketT>
class CConnection;

namespace net
{
/**
 * Acceptor interface for a TCP server.
 * @tparam SocketT The socket implementation
 */
template<typename SocketT>
class IAcceptor
{
public:
    NOT_COPYABLE(IAcceptor)

    using Callback = std::function<void(bool)>;

    IAcceptor()                   = default;
    virtual ~IAcceptor() noexcept = default;

    /// Run the accept loop.
    virtual void Run() = 0;

    /// Stop the accept loop.
    virtual void Stop() = 0;

    /**
     * Execute a callback on accept event.
     * @param cb_ The callback to invoke
     */
    virtual void OnAccept(Callback&& cb_) = 0;

    /// Close the staged connection
    virtual void Close() = 0;

    /**
     * Start the staged connection.
     * @return the connection
     */
    virtual CConnection<SocketT> StartConnection() = 0;

    /// Get the address of the staged connection endpoint.
    virtual String StagedAddress() const = 0;
};
}  // namespace net
}  // namespace vfrb::server
