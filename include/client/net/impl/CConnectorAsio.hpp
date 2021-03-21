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

#include <istream>

#include "client/net/IConnector.hpp"

#include "asio.hpp"

namespace vfrb::client::net
{
class CConnectorAsio : public IConnector
{
    asio::io_context        m_ioCtx;
    asio::ip::tcp::socket   m_socket;
    asio::ip::tcp::resolver m_resolver;
    asio::deadline_timer    m_timer;
    asio::streambuf         m_buffer;
    String                  m_response;
    std::istream            m_istream;

    void
    handleConnect(asio::error_code err_, Callback cb_) noexcept;

    void
    handleTimeout(asio::error_code err_, Callback cb_) noexcept;

    void
    handleRead(asio::error_code err_, usize bytes_, ReadCallback cb_) noexcept;

public:
    CConnectorAsio();

    void
    Run() override;

    void
    Stop() override;

    void
    Close() override;

    void
    OnConnect(SEndpoint const& ep_, Callback cb_) override;

    void
    OnRead(ReadCallback cb_) override;

    void
    OnWrite(String const& str_, Callback cb_) override;

    void
    OnTimeout(Callback cb_, u32 to_) override;

    void
    ResetTimer(u32 to_) override;

    auto
    TimerExpired() -> bool override;
};
}  // namespace vfrb::client::net
