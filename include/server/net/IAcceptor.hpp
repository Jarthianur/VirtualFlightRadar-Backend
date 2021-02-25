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

#include <functional>

#include "util/ClassUtils.hpp"

#include "Types.hpp"

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
    NOT_MOVABLE(IAcceptor)

    using Callback = std::function<void(Result<void>&&)>;

    IAcceptor()                   = default;
    virtual ~IAcceptor() noexcept = default;

    /// Run the accept loop.
    virtual void
    Run() = 0;

    /// Stop the accept loop.
    virtual void
    Stop() = 0;

    /**
     * Execute a callback on accept event.
     * @param cb_ The callback to invoke
     */
    virtual void
    OnAccept(Callback cb_) = 0;

    /// Close the staged connection
    virtual void
    Close() = 0;

    /**
     * Start the staged connection.
     * @return the connection
     */
    virtual auto
    StartConnection() -> CConnection<SocketT> = 0;

    /// Get the address of the staged connection endpoint.
    [[nodiscard]] virtual auto
    StagedAddress() const -> String = 0;
};
}  // namespace net
}  // namespace vfrb::server
