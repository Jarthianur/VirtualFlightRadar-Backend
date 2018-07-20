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
#include "../Logger.hpp"

namespace server
{
TcpInterfaceImplBoost::TcpInterfaceImplBoost(std::uint16_t vPort)
    : mIoService(),
      mAcceptor(mIoService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), vPort),
                boost::asio::ip::tcp::acceptor::reuse_address(true)),
      mSocket(boost::move(boost::asio::ip::tcp::socket(mIoService)))
{}

TcpInterfaceImplBoost::~TcpInterfaceImplBoost() noexcept
{
    stop();
}

void TcpInterfaceImplBoost::run()
{
    mIoService.run();
}

void TcpInterfaceImplBoost::stop()
{
    if(mAcceptor.is_open())
    {
        mAcceptor.close();
    }
    if(!mIoService.stopped())
    {
        mIoService.stop();
    }
    mSocket.close();
}

void TcpInterfaceImplBoost::onAccept(const std::function<void(bool) noexcept>& crCallback)
{
    mAcceptor.async_accept(mSocket.get(),
                           boost::bind(&TcpInterfaceImplBoost::handleAccept, this,
                                       boost::asio::placeholders::error, crCallback));
}

void TcpInterfaceImplBoost::close()
{
    mSocket.close();
}

void TcpInterfaceImplBoost::handleAccept(
    const boost::system::error_code& crError,
    const std::function<void(bool) noexcept>& crCallback) noexcept
{
    if(mAcceptor.is_open())
    {
        if(crError)
        {
            logger.debug("(Server) accept: ", crError.message());
        }
        crCallback(!crError);
    }
}

SocketImplBoost& TcpInterfaceImplBoost::getSocket()
{
    return mSocket;
}

}  // namespace server
