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

#include <boost/bind.hpp>
#include <boost/move/move.hpp>

#include "server/CConnection.hpp"
#include "server/net/impl/CAcceptorBoost.hpp"

#include "CLogger.hpp"

namespace vfrb::server::net
{
constexpr auto     LOG_PREFIX = "(NetworkInterfaceImplBoost) ";
static auto const& logger     = CLogger::Instance();

CAcceptorBoost::CAcceptorBoost(u16 port_)
    : IAcceptor<CSocketBoost>(),
      m_ioService(),
      m_acceptor(m_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_),
                 boost::asio::ip::tcp::acceptor::reuse_address(true)),
      m_socket(boost::move(boost::asio::ip::tcp::socket(m_ioService))) {}

CAcceptorBoost::~CAcceptorBoost() noexcept {
    Stop();
}

void CAcceptorBoost::Run() {
    try {
        if (m_acceptor.is_open()) {
            m_ioService.run();
        }
    } catch (std::exception const& e) {
        logger.Error(LOG_PREFIX, ": ", e.what());
    } catch (...) {
        logger.Error(LOG_PREFIX, ": unknown error");
    }
}

void CAcceptorBoost::Stop() {
    if (m_acceptor.is_open()) {
        m_acceptor.close();
    }
    if (!m_ioService.stopped()) {
        m_ioService.stop();
    }
    m_socket.Close();
}

void CAcceptorBoost::OnAccept(Callback&& cb_) {
    if (m_acceptor.is_open()) {
        m_acceptor.async_accept(m_socket.Get(), boost::bind(&CAcceptorBoost::handleAccept, this,
                                                            boost::asio::placeholders::error, cb_));
    }
}

void CAcceptorBoost::Close() {
    m_socket.Close();
}

void CAcceptorBoost::handleAccept(boost::system::error_code const& err_, Callback const& cb_) {
    if (err_) {
        logger.Debug(LOG_PREFIX, "accept: ", err_.message());
    }
    cb_(bool(err_));
}

CConnection<CSocketBoost> CAcceptorBoost::StartConnection() {
    if (!m_socket.Get().is_open()) {
        throw error::CSocketError("cannot start connection on closed socket");
    }
    return CConnection<CSocketBoost>(std::move(m_socket));
}

String CAcceptorBoost::StagedAddress() const {
    return m_socket.Address();
}
}  // namespace vfrb::server::net
