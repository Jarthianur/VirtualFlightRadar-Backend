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

#include "client/net/impl/CConnectorAsio.hpp"

#include "boost/date_time.hpp"
#include "client/net/SEndpoint.hpp"

#include "CLogger.hpp"

using asio::error_code;
using asio::ip::tcp;
using asio::buffer;
using asio::deadline_timer;
using asio::socket_base;
using std::literals::operator""s;

namespace vfrb::client::net
{
static auto const& logger = CLogger::Instance();

CConnectorAsio::CConnectorAsio()
    : m_ioCtx(), m_socket(m_ioCtx), m_resolver(m_ioCtx), m_timer(m_ioCtx), m_istream(&m_buffer) {}

void
CConnectorAsio::Run() {
    m_ioCtx.run();
}

void
CConnectorAsio::Stop() {
    Close();
    if (!m_ioCtx.stopped()) {
        m_ioCtx.stop();
    }
}

void
CConnectorAsio::Close() {
    m_timer.expires_at(boost::posix_time::pos_infin);
    m_timer.cancel();
    if (m_socket.is_open()) {
        error_code ec;
        m_socket.shutdown(tcp::socket::shutdown_both, ec);
        m_socket.close();
    }
}

void
CConnectorAsio::OnConnect(SEndpoint const& ep_, Callback cb_) {
    m_resolver.async_resolve(
        ep_.Host, ep_.Port, tcp::resolver::query::canonical_name,
        [this, cb{std::move(cb_)}](error_code err_, tcp::resolver::results_type res_) {
            if (!err_) {
                async_connect(m_socket, res_,
                              [this, cb{std::move(cb)}](error_code err_, [[maybe_unused]] tcp::endpoint) {
                                  handleConnect(err_, cb);
                              });
            } else {
                logger.Debug("(Client) failed to resolve host: ", err_.message());
                cb(Err());
            }
        });
}

void
CConnectorAsio::OnRead(ReadCallback cb_) {
    if (m_socket.is_open()) {
        async_read_until(m_socket, m_buffer, "\r\n",
                         [this, cb{std::move(cb_)}](error_code err_, usize b_) { handleRead(err_, b_, cb); });
    }
}

void
CConnectorAsio::OnWrite(String const& str_, Callback cb_) {
    if (m_socket.is_open()) {
        error_code err;
        asio::write(m_socket, buffer(str_), err);
        if (err) {
            logger.Debug("(Client) failed to write: ", err.message());
            cb_(Err());
        } else {
            cb_(Ok());
        }
    }
}

void
CConnectorAsio::OnTimeout(Callback cb_, u32 to_) {
    if (to_ > 0) {
        ResetTimer(to_);
    }
    m_timer.async_wait([this, cb{std::move(cb_)}](error_code err_) { handleTimeout(err_, cb); });
}

void
CConnectorAsio::ResetTimer(u32 to_) {
    m_timer.expires_from_now(boost::posix_time::seconds(to_));
}

auto
CConnectorAsio::TimerExpired() -> bool {
    return m_timer.expires_at() <= deadline_timer::traits_type::now();
}

void
CConnectorAsio::handleConnect(error_code err_, Callback cb_) noexcept {
    if (!err_) {
        m_socket.set_option(socket_base::keep_alive(true));
        cb_(Ok());
    } else {
        logger.Debug("(Client) failed to connect: ", err_.message());
        cb_(Err());
    }
}

void
CConnectorAsio::handleTimeout(error_code err_, Callback cb_) noexcept {
    if (err_) {
        logger.Debug("(Client) timeout: ", err_.message());
        cb_(Err());
    } else {
        cb_(Ok());
    }
}

void
CConnectorAsio::handleRead(error_code err_, [[maybe_unused]] usize, ReadCallback cb_) noexcept {
    if (!err_) {
        std::getline(m_istream, m_response);
        m_response.append("\n");
        cb_(Ok(std::move(m_response)));
    } else {
        logger.Debug("(Client) read: ", err_.message());
        cb_(Err(""s));
    }
}
}  // namespace vfrb::client::net
