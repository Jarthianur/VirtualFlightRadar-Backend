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

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "server/net/Acceptor.hpp"
#include "server/net/impl/SocketBoost.h"
#include "util/class_utils.h"

#include "types.h"

namespace vfrb::server::net
{
/**
 * @brief Implement NetworkInterface using boost.
 */
class CAcceptorBoost : public IAcceptor<CSocketBoost>
{
    NOT_COPYABLE(CAcceptorBoost)

    boost::asio::io_service        m_ioService;  ///< Internal IO-service
    boost::asio::ip::tcp::acceptor m_acceptor;   ///< Internal acceptor
    CSocketBoost                   m_socket;     ///< Staging socket

    /// Intermediate handler for accept event
    void handleAccept(boost::system::error_code const& err_, Callback const& cb_);

public:
    explicit CAcceptorBoost(u16 port_);
    ~CAcceptorBoost() noexcept override;

    void Run() override;
    void Stop() override;
    void OnAccept(Callback&& cb_) override;
    void Close() override;

    /// @throw vfrb::server::net::error::CSocketError
    CConnection<CSocketBoost> StartConnection() override;
    Str                       StagedAddress() const override;
};
}  // namespace vfrb::server::net
