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

#include "ConnectorImplBoost.h"

#include <boost/bind.hpp>
#include <boost/date_time.hpp>
#include <boost/move/move.hpp>

#include "../util/Logger.hpp"
#include "Endpoint.hpp"

namespace client
{
ConnectorImplBoost::ConnectorImplBoost()
    : Connector(),
      m_ioService(),
      m_socket(m_ioService),
      m_resolver(m_ioService),
      m_timer(m_ioService),
      m_istream(&m_buffer)
{}

ConnectorImplBoost::~ConnectorImplBoost() noexcept
{}

void ConnectorImplBoost::run()
{
    m_ioService.run();
}

void ConnectorImplBoost::stop()
{
    close();
    if(!m_ioService.stopped())
    {
        m_ioService.stop();
    }
}

void ConnectorImplBoost::close()
{
    m_timer.expires_at(boost::posix_time::pos_infin);
    m_timer.cancel();
    if(m_socket.is_open())
    {
        boost::system::error_code ec;
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        m_socket.close();
    }
}

void ConnectorImplBoost::onConnect(const Endpoint& endpoint, const Callback& callback)
{
    boost::asio::ip::tcp::resolver::query query(
        endpoint.host, endpoint.port, boost::asio::ip::tcp::resolver::query::canonical_name);
    m_resolver.async_resolve(query, boost::bind(&ConnectorImplBoost::handleResolve, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::iterator, callback));
}

void ConnectorImplBoost::onRead(const ReadCallback& callback)
{
    if(m_socket.is_open())
    {
        boost::asio::async_read_until(
            m_socket, m_buffer, "\r\n",
            boost::bind(&ConnectorImplBoost::handleRead, this, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred, callback));
    }
}

void ConnectorImplBoost::onWrite(const std::string& msg, const Callback& callback)
{
    if(m_socket.is_open())
    {
        boost::asio::async_write(
            m_socket, boost::asio::buffer(msg),
            boost::bind(&ConnectorImplBoost::handleWrite, this, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred, callback));
    }
}

void ConnectorImplBoost::onTimeout(const Callback& callback, std::uint32_t timeout)
{
    if(timeout > 0)
    {
        resetTimer(timeout);
    }
    m_timer.async_wait(boost::bind(&ConnectorImplBoost::handleTimeout, this,
                                   boost::asio::placeholders::error, callback));
}

void ConnectorImplBoost::resetTimer(std::uint32_t vTimeout)
{
    m_timer.expires_from_now(boost::posix_time::seconds(vTimeout));
}

bool ConnectorImplBoost::timerExpired()
{
    return m_timer.expires_at() <= boost::asio::deadline_timer::traits_type::now();
}

void ConnectorImplBoost::handleWrite(const boost::system::error_code& error, std::size_t,
                                     const Callback& callback) noexcept
{
    if(error)
    {
        logger.debug("(Client) failed to write: ", error.message());
    }
    callback(error);
}

void ConnectorImplBoost::handleResolve(const boost::system::error_code& error,
                                       boost::asio::ip::tcp::resolver::iterator resolverIt,
                                       const Callback& callback) noexcept
{
    if(!error)
    {
        boost::asio::async_connect(m_socket, resolverIt,
                                   boost::bind(&ConnectorImplBoost::handleConnect, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::iterator, callback));
    }
    else
    {
        logger.debug("(Client) failed to resolve host: ", error.message());
        callback(true);
    }
}

void ConnectorImplBoost::handleConnect(const boost::system::error_code& error,
                                       boost::asio::ip::tcp::resolver::iterator,
                                       const Callback& callback) noexcept
{
    if(error)
    {
        logger.debug("(Client) failed to connect: ", error.message());
    }
    else
    {
        m_socket.set_option(boost::asio::socket_base::keep_alive(true));
    }
    callback(error);
}

void ConnectorImplBoost::handleTimeout(const boost::system::error_code& error,
                                       const Callback& callback) noexcept
{
    if(error)
    {
        logger.debug("(Client) timeout: ", error.message());
    }
    callback(error);
}

void ConnectorImplBoost::handleRead(const boost::system::error_code& error, std::size_t,
                                    const ReadCallback& callback) noexcept
{
    if(error)
    {
        logger.debug("(Client) read: ", error.message());
    }
    else
    {
        std::getline(m_istream, m_response);
        m_response.append("\n");
    }
    callback(error, m_response);
}

}  // namespace client
