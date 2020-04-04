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

#include "client/net/impl/CConnectorBoost.hpp"

#include <boost/bind.hpp>
#include <boost/date_time.hpp>
#include <boost/move/move.hpp>

#include "client/net/SEndpoint.hpp"

#include "CLogger.hpp"

using namespace boost::asio;
using namespace boost::system;

namespace vfrb::client::net
{
static auto const& logger = CLogger::Instance();

CConnectorBoost::CConnectorBoost()
    : m_ioCtx(), m_socket(m_ioCtx), m_resolver(m_ioCtx), m_timer(m_ioCtx), m_istream(&m_buffer) {}

EErrc CConnectorBoost::evalErrorCode(boost::system::error_code const& err_) const {
    return !err_ ? EErrc::OK : EErrc::Err;
}

void CConnectorBoost::Run() {
    m_ioCtx.run();
}

void CConnectorBoost::Stop() {
    Close();
    if (!m_ioCtx.stopped()) {
        m_ioCtx.stop();
    }
}

void CConnectorBoost::Close() {
    m_timer.expires_at(boost::posix_time::pos_infin);
    m_timer.cancel();
    if (m_socket.is_open()) {
        error_code ec;
        m_socket.shutdown(ip::tcp::socket::shutdown_both, ec);
        m_socket.close();
    }
}

void CConnectorBoost::OnConnect(SEndpoint const& ep_, Callback const& cb_) {
    m_resolver.async_resolve(
        ep_.Host, ep_.Port, ip::tcp::resolver::query::canonical_name,
        boost::bind(&CConnectorBoost::handleResolve, this, placeholders::error, placeholders::results, cb_));
}

void CConnectorBoost::OnRead(ReadCallback const& cb_) {
    if (m_socket.is_open()) {
        async_read_until(m_socket, m_buffer, "\r\n",
                         boost::bind(&CConnectorBoost::handleRead, this, placeholders::error,
                                     placeholders::bytes_transferred, cb_));
    }
}

void CConnectorBoost::OnWrite(String const& str_, Callback const& cb_) {
    if (m_socket.is_open()) {
        async_write(m_socket, buffer(str_),
                    boost::bind(&CConnectorBoost::handleWrite, this, placeholders::error,
                                placeholders::bytes_transferred, cb_));
    }
}

void CConnectorBoost::OnTimeout(Callback const& cb_, u32 to_) {
    if (to_ > 0) {
        ResetTimer(to_);
    }
    m_timer.async_wait(boost::bind(&CConnectorBoost::handleTimeout, this, placeholders::error, cb_));
}

void CConnectorBoost::ResetTimer(u32 to_) {
    m_timer.expires_from_now(boost::posix_time::seconds(to_));
}

bool CConnectorBoost::TimerExpired() {
    return m_timer.expires_at() <= deadline_timer::traits_type::now();
}

void CConnectorBoost::handleWrite(error_code const& err_, [[maybe_unused]] usize,
                                  Callback const&   cb_) noexcept {
    EErrc const ec = evalErrorCode(err_);
    if (ec != EErrc::OK) {
        logger.Debug("(Client) failed to write: ", err_.message());
    }
    cb_(ec);
}

void CConnectorBoost::handleResolve(error_code const& err_, ip::tcp::resolver::results_type res_,
                                    Callback const& cb_) noexcept {
    EErrc const ec = evalErrorCode(err_);
    if (ec == EErrc::OK) {
        async_connect(m_socket, res_,
                      boost::bind(&CConnectorBoost::handleConnect, this, placeholders::error, cb_));
    } else {
        logger.Debug("(Client) failed to resolve host: ", err_.message());
        cb_(ec);
    }
}

void CConnectorBoost::handleConnect(error_code const& err_, Callback const& cb_) noexcept {
    EErrc const ec = evalErrorCode(err_);
    if (ec == EErrc::OK) {
        m_socket.set_option(socket_base::keep_alive(true));
    } else {
        logger.Debug("(Client) failed to connect: ", err_.message());
    }
    cb_(ec);
}

void CConnectorBoost::handleTimeout(error_code const& err_, Callback const& cb_) noexcept {
    EErrc const ec = evalErrorCode(err_);
    if (ec != EErrc::OK) {
        logger.Debug("(Client) timeout: ", err_.message());
    }
    cb_(ec);
}

void CConnectorBoost::handleRead(error_code const&   err_, [[maybe_unused]] usize,
                                 ReadCallback const& cb_) noexcept {
    EErrc const ec = evalErrorCode(err_);
    if (ec == EErrc::OK) {
        std::getline(m_istream, m_response);
        m_response.append("\n");
    } else {
        logger.Debug("(Client) read: ", err_.message());
    }
    cb_(ec, m_response);
}
}  // namespace vfrb::client::net
