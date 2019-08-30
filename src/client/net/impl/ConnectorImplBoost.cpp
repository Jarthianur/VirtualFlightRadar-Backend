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
static auto const& logger = Logger::instance();

ConnectorImplBoost::ConnectorImplBoost()
    : m_ioService(),
      m_socket(m_ioService),
      m_resolver(m_ioService),
      m_timer(m_ioService),
      m_istream(&m_buffer)
{}

void ConnectorImplBoost::run()
{
    m_ioService.run();
}

void ConnectorImplBoost::stop()
{
    close();
    if (!m_ioService.stopped())
    {
        m_ioService.stop();
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
    ip::tcp::resolver::query query(endpoint.host, endpoint.port, ip::tcp::resolver::query::canonical_name);
    m_resolver.async_resolve(query, boost::bind(&ConnectorImplBoost::handleResolve, this, placeholders::error,
                                                placeholders::iterator, callback));
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

void ConnectorImplBoost::onWrite(str const& msg, Callback const& callback)
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
    if (error)
    {
        logger.debug("(Client) failed to write: ", error.message());
    }
    callback(bool(error));
}

void ConnectorImplBoost::handleResolve(error_code const& error, ip::tcp::resolver::iterator resolverIt,
                                       Callback const& callback) noexcept
{
    if (!error)
    {
        async_connect(m_socket, resolverIt,
                      boost::bind(&ConnectorImplBoost::handleConnect, this, placeholders::error,
                                  placeholders::iterator, callback));
    }
    else
    {
        logger.debug("(Client) failed to resolve host: ", error.message());
        callback(true);
    }
}

void ConnectorImplBoost::handleConnect(error_code const& error, [[maybe_unused]] ip::tcp::resolver::iterator,
                                       Callback const&   callback) noexcept
{
    if (error)
    {
        logger.debug("(Client) failed to connect: ", error.message());
    }
    else
    {
        m_socket.set_option(socket_base::keep_alive(true));
    }
    callback(bool(error));
}

void ConnectorImplBoost::handleTimeout(error_code const& error, Callback const& callback) noexcept
{
    if (error)
    {
        logger.debug("(Client) timeout: ", error.message());
    }
    callback(bool(error));
}

void ConnectorImplBoost::handleRead(error_code const&   error, [[maybe_unused]] usize,
                                    ReadCallback const& callback) noexcept
{
    if (error)
    {
        logger.debug("(Client) read: ", error.message());
    }
    else
    {
        std::getline(m_istream, m_response);
        m_response.append("\n");
    }
    callback(bool(error), m_response);
}
}  // namespace vfrb::client::net
