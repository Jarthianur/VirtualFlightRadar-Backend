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

#include "server/net/impl/NetworkInterfaceImplBoost.h"

#include <boost/bind.hpp>
#include <boost/move/move.hpp>

#include "server/Connection.hpp"
#include "util/Logger.hpp"

namespace server::net
{
constexpr auto     LOG_PREFIX = "(NetworkInterfaceImplBoost) ";
static auto const& logger     = Logger::instance();

NetworkInterfaceImplBoost::NetworkInterfaceImplBoost(u16 port)
    : NetworkInterface<SocketImplBoost>(),
      m_ioService(),
      m_acceptor(m_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port),
                 boost::asio::ip::tcp::acceptor::reuse_address(true)),
      m_socket(boost::move(boost::asio::ip::tcp::socket(m_ioService)))
{}

NetworkInterfaceImplBoost::~NetworkInterfaceImplBoost() noexcept
{
    stop();
}

void NetworkInterfaceImplBoost::run(std::unique_lock<std::mutex>& lk)
{
    try
    {
        if (m_acceptor.is_open())
        {
            lk.unlock();
            m_ioService.run();
        }
    }
    catch (std::exception const& e)
    {
        logger.error(LOG_PREFIX, ": ", e.what());
    }
    catch (...)
    {
        logger.error(LOG_PREFIX, ": unknown error");
    }
}

void NetworkInterfaceImplBoost::stop()
{
    if (m_acceptor.is_open())
    {
        m_acceptor.close();
    }
    if (!m_ioService.stopped())
    {
        m_ioService.stop();
    }
    m_socket.close();
}

void NetworkInterfaceImplBoost::onAccept(Callback const& callback)
{
    if (m_acceptor.is_open())
    {
        m_acceptor.async_accept(m_socket.get(), boost::bind(&NetworkInterfaceImplBoost::handleAccept, this,
                                                            boost::asio::placeholders::error, callback));
    }
}

void NetworkInterfaceImplBoost::close()
{
    m_socket.close();
}

void NetworkInterfaceImplBoost::handleAccept(boost::system::error_code const& error, Callback const& callback)
{
    if (error)
    {
        logger.debug(LOG_PREFIX, "accept: ", error.message());
    }
    callback(bool(error));
}

u_ptr<Connection<SocketImplBoost>> NetworkInterfaceImplBoost::startConnection()
{
    if (!m_socket.get().is_open())
    {
        throw SocketException("cannot start connection on closed socket");
    }
    return Connection<SocketImplBoost>::create(std::move(m_socket));
}

str NetworkInterfaceImplBoost::stagedAddress() const
{
    return m_socket.address();
}
}  // namespace server::net
