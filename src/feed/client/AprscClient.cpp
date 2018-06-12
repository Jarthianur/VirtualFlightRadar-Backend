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

#include "AprscClient.h"

#include <boost/bind.hpp>
#include <boost/date_time.hpp>

#include "../../Logger.hpp"

#define COMPONENT "(AprscClient)"

namespace feed
{
namespace client
{
AprscClient::AprscClient(const std::string& crHost, const std::string& crPort,
                         const std::string& crLogin, feed::Feed& rFeed)
    : Client(crHost, crPort, COMPONENT, rFeed),
      mLoginStr(crLogin),
      mStopped(false),
      mTimeout(mIoService, boost::posix_time::minutes(10))
{
    mLoginStr.append("\r\n");
    connect();
}

AprscClient::~AprscClient() noexcept
{}

void AprscClient::connect()
{
    boost::asio::ip::tcp::resolver::query query(
        mHost, mPort, boost::asio::ip::tcp::resolver::query::canonical_name);
    mResolver.async_resolve(query, boost::bind(&AprscClient::handleResolve, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::iterator));
}

void AprscClient::stop()
{
    Client::stop();
    mStopped = true;
    mTimeout.expires_at(boost::posix_time::pos_infin);
    mTimeout.cancel();
}

void AprscClient::handleResolve(
    const boost::system::error_code& crError,
    boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept
{
    if(!crError)
    {
        boost::asio::async_connect(mSocket, vResolverIt,
                                   boost::bind(&AprscClient::handleConnect, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::iterator));
    }
    else
    {
        Logger::error(COMPONENT " resolve host: ", crError.message());
        if(mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void AprscClient::handleConnect(const boost::system::error_code& crError,
                                boost::asio::ip::tcp::resolver::iterator) noexcept
{
    if(!crError)
    {
        mSocket.set_option(boost::asio::socket_base::keep_alive(true));
        boost::asio::async_write(
            mSocket, boost::asio::buffer(mLoginStr),
            boost::bind(&AprscClient::handleLogin, this, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        mTimeout.async_wait(boost::bind(&AprscClient::sendKeepAlive, this));
    }
    else
    {
        Logger::error(COMPONENT " connect: ", crError.message());
        if(mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void AprscClient::sendKeepAlive()
{
    if(mStopped)
    {
        return;
    }
    boost::asio::async_write(mSocket, boost::asio::buffer("#keep-alive beacon\r\n"),
                             boost::bind(&AprscClient::handleSendKeepAlive, this,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
    mTimeout.expires_from_now(boost::posix_time::minutes(10));
    mTimeout.async_wait(boost::bind(&AprscClient::sendKeepAlive, this));
}

void AprscClient::handleLogin(const boost::system::error_code& crError,
                              std::size_t) noexcept
{
    if(!crError)
    {
        Logger::info(COMPONENT " connected to: ", mHost, ":", mPort);
        read();
    }
    else
    {
        Logger::error(COMPONENT " send login: ", crError.message());
    }
}

void AprscClient::handleSendKeepAlive(const boost::system::error_code& crError,
                                      std::size_t) noexcept
{
    if(crError)
    {
        Logger::error(COMPONENT " send beacon:", crError.message());
    }
}

}  // namespace client
}  // namespace feed
