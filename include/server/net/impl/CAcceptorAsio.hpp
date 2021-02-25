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

#include "server/net/IAcceptor.hpp"
#include "server/net/impl/CSocketAsio.hpp"
#include "util/ClassUtils.hpp"

#include "Types.hpp"

namespace vfrb::server::net
{
/**
 * @brief Implement NetworkInterface using boost.
 */
class CAcceptorAsio : public IAcceptor<CSocketAsio>
{
    NOT_COPYABLE(CAcceptorAsio)
    NOT_MOVABLE(CAcceptorAsio)

    asio::io_context        m_ioCtx;     ///< Internal IO-service
    asio::ip::tcp::acceptor m_acceptor;  ///< Internal acceptor
    CSocketAsio             m_socket;    ///< Staging socket

    /// Intermediate handler for accept event
    void
    handleAccept(asio::error_code err_, Callback cb_);

public:
    explicit CAcceptorAsio(u16 port_);
    ~CAcceptorAsio() noexcept override;

    void
    Run() override;

    void
    Stop() override;

    void
    OnAccept(Callback cb_) override;

    void
    Close() override;

    /// @throw vfrb::server::net::error::CSocketError
    auto
    StartConnection() -> CConnection<CSocketAsio> override;

    [[nodiscard]] auto
    StagedAddress() const -> String override;
};
}  // namespace vfrb::server::net
