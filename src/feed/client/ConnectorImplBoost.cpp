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

#include <istream>
#include <boost/bind.hpp>
#include <boost/move/move.hpp>

#include "../../Logger.hpp"

namespace feed
{
namespace client
{
ConnectorImplBoost::ConnectorImplBoost()
    : mIoService(), mSocket(mIoService), mResolver(mIoService), mConnectTimer(mIoService)
{}

ConnectorImplBoost::~ConnectorImplBoost() noexcept
{}

void ConnectorImplBoost::run()
{
    mIoService.run();
}

void ConnectorImplBoost::stop()
{
    close();
    if(!mIoService.stopped())
    {
        mIoService.stop();
    }
}

void ConnectorImplBoost::close()
{
    mConnectTimer.expires_at(boost::posix_time::pos_infin);
    mConnectTimer.cancel();
    if(mSocket.is_open())
    {
        boost::system::error_code ec;
        mSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        mSocket.close();
    }
}

void ConnectorImplBoost::onRead(
    const std::function<void(bool, const std::string&) noexcept>& crCallback)
{
    boost::asio::async_read_until(
        mSocket, mBuffer, "\r\n",
        boost::bind(&ConnectorImplBoost::handleRead, this, boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred, crCallback));
}

void ConnectorImplBoost::onTimeout(std::uint32_t vTimeout,
                                   const std::function<void(bool) noexcept>& crCallback)
{
    mConnectTimer.expires_from_now(boost::posix_time::seconds(vTimeout));
    mConnectTimer.async_wait(boost::bind(&ConnectorImplBoost::handleTimeout, this,
                                         boost::asio::placeholders::error, crCallback));
}

void ConnectorImplBoost::handleTimeout(
    const boost::system::error_code& crError,
    const std::function<void(bool) noexcept>& crCallback) noexcept
{
    if(crError)
    {
        logger.debug("(Client) timeout: ", crError.message());
    }
    crCallback(!crError);
}

void ConnectorImplBoost::handleRead(
    const boost::system::error_code& crError, std::size_t,
    const std::function<void(bool, const std::string&) noexcept>& crCallback) noexcept
{
    if(crError)
    {
        logger.debug("(Client) read: ", crError.message());
    }
    else
    {
        std::istream is(&mBuffer);
        std::getline(is, mResponse);
        mResponse.append("\n");
    }
    crCallback(!crError, mResponse);
}

}  // namespace client
}  // namespace feed
