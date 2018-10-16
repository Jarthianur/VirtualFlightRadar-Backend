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

#include "TcpInterfaceImplBoost.h"

#include <boost/bind.hpp>
#include <boost/move/move.hpp>

#include "../util/Logger.hpp"

#include "Connection.hpp"

namespace server
{
TcpInterfaceImplBoost::TcpInterfaceImplBoost(std::uint16_t port)
    : TcpInterface<SocketImplBoost>(),
      m_ioService(),
      m_acceptor(m_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port),
                 boost::asio::ip::tcp::acceptor::reuse_address(true)),
      m_socket(boost::move(boost::asio::ip::tcp::socket(m_ioService)))
{}

TcpInterfaceImplBoost::~TcpInterfaceImplBoost() noexcept
{
    stop();
}

void TcpInterfaceImplBoost::run(std::unique_lock<std::mutex>& lock)
{
    try
    {
        if (m_acceptor.is_open())
        {
            lock.unlock();
            m_ioService.run();
        }
    }
    catch (const std::exception& e)
    {
        logger.error("TcpInterfaceImplBoost::run() caught: ", e.what());
    }
    catch (...)
    {
        logger.error("TcpInterfaceImplBoost::run() caught error");
    }
}

void TcpInterfaceImplBoost::stop()
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

void TcpInterfaceImplBoost::onAccept(const std::function<void(bool)>& callback)
{
    if (m_acceptor.is_open())
    {
        m_acceptor.async_accept(m_socket.get(),
                                boost::bind(&TcpInterfaceImplBoost::handleAccept, this,
                                            boost::asio::placeholders::error, callback));
    }
}

void TcpInterfaceImplBoost::close()
{
    m_socket.close();
}

void TcpInterfaceImplBoost::handleAccept(const boost::system::error_code& error,
                                         const std::function<void(bool)>& callback)
{
    if (error)
    {
        logger.debug("(Server) accept: ", error.message());
    }
    callback(error);
}

std::unique_ptr<Connection<SocketImplBoost>> TcpInterfaceImplBoost::startConnection()
{
    if (!m_socket.get().is_open())
    {
        throw SocketException("cannot start connection on closed socket");
    }
    return Connection<SocketImplBoost>::create(std::move(m_socket));
}

std::string TcpInterfaceImplBoost::get_currentAddress() const
{
    return m_socket.get_address();
}

}  // namespace server
