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

#include "client/net/impl/ConnectorImplBoost.h"

#include <boost/bind.hpp>
#include <boost/date_time.hpp>
#include <boost/move/move.hpp>

#include "client/net/Endpoint.hpp"
#include "util/Logger.hpp"

using namespace boost::asio;
using namespace boost::system;

namespace vfrb::client::net
{
static auto const& logger = CLogger::Instance();

ConnectorImplBoost::ConnectorImplBoost()
    : m_ioCtx(), m_socket(m_ioCtx), m_resolver(m_ioCtx), m_timer(m_ioCtx), m_istream(&m_buffer)
{}

ErrorCode ConnectorImplBoost::evalErrorCode(boost::system::error_code const& error) const
{
    return !error ? ErrorCode::SUCCESS : ErrorCode::FAILURE;
}

void ConnectorImplBoost::run()
{
    m_ioCtx.run();
}

void ConnectorImplBoost::stop()
{
    close();
    if (!m_ioCtx.stopped())
    {
        m_ioCtx.stop();
    }
}

void ConnectorImplBoost::close()
{
    m_timer.expires_at(boost::posix_time::pos_infin);
    m_timer.cancel();
    if (m_socket.is_open())
    {
        error_code ec;
        m_socket.shutdown(ip::tcp::socket::shutdown_both, ec);
        m_socket.close();
    }
}

void ConnectorImplBoost::onConnect(Endpoint const& endpoint, Callback const& callback)
{
    m_resolver.async_resolve(endpoint.host, endpoint.port, ip::tcp::resolver::query::canonical_name,
                             boost::bind(&ConnectorImplBoost::handleResolve, this, placeholders::error,
                                         placeholders::results, callback));
}

void ConnectorImplBoost::onRead(ReadCallback const& callback)
{
    if (m_socket.is_open())
    {
        async_read_until(m_socket, m_buffer, "\r\n",
                         boost::bind(&ConnectorImplBoost::handleRead, this, placeholders::error,
                                     placeholders::bytes_transferred, callback));
    }
}

void ConnectorImplBoost::onWrite(Str const& msg, Callback const& callback)
{
    if (m_socket.is_open())
    {
        async_write(m_socket, buffer(msg),
                    boost::bind(&ConnectorImplBoost::handleWrite, this, placeholders::error,
                                placeholders::bytes_transferred, callback));
    }
}

void ConnectorImplBoost::onTimeout(Callback const& callback, u32 timeout)
{
    if (timeout > 0)
    {
        resetTimer(timeout);
    }
    m_timer.async_wait(boost::bind(&ConnectorImplBoost::handleTimeout, this, placeholders::error, callback));
}

void ConnectorImplBoost::resetTimer(u32 vTimeout)
{
    m_timer.expires_from_now(boost::posix_time::seconds(vTimeout));
}

bool ConnectorImplBoost::timerExpired()
{
    return m_timer.expires_at() <= deadline_timer::traits_type::now();
}

void ConnectorImplBoost::handleWrite(error_code const& error, [[maybe_unused]] usize,
                                     Callback const&   callback) noexcept
{
    ErrorCode const ec = evalErrorCode(error);
    if (ec != ErrorCode::SUCCESS)
    {
        logger.debug("(Client) failed to write: ", error.message());
    }
    callback(ec);
}

void ConnectorImplBoost::handleResolve(error_code const& error, ip::tcp::resolver::results_type results,
                                       Callback const& callback) noexcept
{
    ErrorCode const ec = evalErrorCode(error);
    if (ec == ErrorCode::SUCCESS)
    {
        async_connect(m_socket, results,
                      boost::bind(&ConnectorImplBoost::handleConnect, this, placeholders::error, callback));
    }
    else
    {
        logger.debug("(Client) failed to resolve host: ", error.message());
        callback(ec);
    }
}

void ConnectorImplBoost::handleConnect(error_code const& error, Callback const& callback) noexcept
{
    ErrorCode const ec = evalErrorCode(error);
    if (ec == ErrorCode::SUCCESS)
    {
        m_socket.set_option(socket_base::keep_alive(true));
    }
    else
    {
        logger.debug("(Client) failed to connect: ", error.message());
    }
    callback(ec);
}

void ConnectorImplBoost::handleTimeout(error_code const& error, Callback const& callback) noexcept
{
    ErrorCode const ec = evalErrorCode(error);
    if (ec != ErrorCode::SUCCESS)
    {
        logger.debug("(Client) timeout: ", error.message());
    }
    callback(ec);
}

void ConnectorImplBoost::handleRead(error_code const&   error, [[maybe_unused]] usize,
                                    ReadCallback const& callback) noexcept
{
    ErrorCode const ec = evalErrorCode(error);
    if (ec == ErrorCode::SUCCESS)
    {
        std::getline(m_istream, m_response);
        m_response.append("\n");
    }
    else
    {
        logger.debug("(Client) read: ", error.message());
    }
    callback(ec, m_response);
}
}  // namespace vfrb::client::net
