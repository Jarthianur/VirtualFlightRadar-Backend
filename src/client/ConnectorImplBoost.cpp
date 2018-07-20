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

#include "../Logger.hpp"
#include "Client.hpp"

namespace client
{
ConnectorImplBoost::ConnectorImplBoost()
    : mIoService(),
      mSocket(mIoService),
      mResolver(mIoService),
      mTimer(mIoService),
      mIStream(&mBuffer)
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
    mTimer.expires_at(boost::posix_time::pos_infin);
    mTimer.cancel();
    if(mSocket.is_open())
    {
        boost::system::error_code ec;
        mSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        mSocket.close();
    }
}

void ConnectorImplBoost::onConnect(const Endpoint& crEndpoint, const Callback& crCallback)
{
    boost::asio::ip::tcp::resolver::query query(
        crEndpoint.host, crEndpoint.port, boost::asio::ip::tcp::resolver::query::canonical_name);
    mResolver.async_resolve(query, boost::bind(&ConnectorImplBoost::handleResolve, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::iterator, crCallback));
}

void ConnectorImplBoost::onRead(const ReadCallback& crCallback)
{
    boost::asio::async_read_until(
        mSocket, mBuffer, "\r\n",
        boost::bind(&ConnectorImplBoost::handleRead, this, boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred, crCallback));
}

void ConnectorImplBoost::onWrite(const std::string& crStr, const Callback& crCallback)
{
    boost::asio::async_write(mSocket, boost::asio::buffer(crStr),
                             boost::bind(&ConnectorImplBoost::handleWrite, this,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred, crCallback));
}

void ConnectorImplBoost::onTimeout(const Callback& crCallback, std::uint32_t vTimeout)
{
    if(vTimeout > 0)
    {
        resetTimer(vTimeout);
    }
    mTimer.async_wait(boost::bind(&ConnectorImplBoost::handleTimeout, this,
                                  boost::asio::placeholders::error, crCallback));
}

void ConnectorImplBoost::resetTimer(std::uint32_t vTimeout)
{
    mTimer.expires_from_now(boost::posix_time::seconds(vTimeout));
}

bool ConnectorImplBoost::timerExpired()
{
    return mTimer.expires_at() <= boost::asio::deadline_timer::traits_type::now();
}

void ConnectorImplBoost::handleWrite(const boost::system::error_code& crError, std::size_t,
                                     const Callback& crCallback) noexcept
{
    if(crError)
    {
        logger.debug("(Client) failed to write: ", crError.message());
    }
    crCallback(!crError);
}

void ConnectorImplBoost::handleResolve(const boost::system::error_code& crError,
                                       boost::asio::ip::tcp::resolver::iterator vResolverIt,
                                       const Callback& crCallback) noexcept
{
    if(!crError)
    {
        boost::asio::async_connect(mSocket, vResolverIt,
                                   boost::bind(&ConnectorImplBoost::handleConnect, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::iterator, crCallback));
    }
    else
    {
        logger.debug("(Client) failed to resolve host: ", crError.message());
        crCallback(false);
    }
}

void ConnectorImplBoost::handleConnect(const boost::system::error_code& crError,
                                       boost::asio::ip::tcp::resolver::iterator,
                                       const Callback& crCallback) noexcept
{
    if(crError)
    {
        logger.debug("(Client) failed to connect: ", crError.message());
    }
    else
    {
        mSocket.set_option(boost::asio::socket_base::keep_alive(true));
    }
    crCallback(!crError);
}

void ConnectorImplBoost::handleTimeout(const boost::system::error_code& crError,
                                       const Callback& crCallback) noexcept
{
    if(crError)
    {
        logger.debug("(Client) timeout: ", crError.message());
    }
    crCallback(!crError);
}

void ConnectorImplBoost::handleRead(const boost::system::error_code& crError, std::size_t,
                                    const ReadCallback& crCallback) noexcept
{
    if(crError)
    {
        logger.debug("(Client) read: ", crError.message());
    }
    else
    {
        std::getline(mIStream, mResponse);
        // mIStream.clear();
        mResponse.append("\n");
        logger.debug("client got: ", mResponse);
    }
    crCallback(!crError, mResponse);
}

}  // namespace client
