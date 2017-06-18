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

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <boost/date_time.hpp>
#include <boost/operators.hpp>
#include <cstddef>
#include <iostream>
#include "../../feed/Feed.h"
#include "../../util/Logger.h"

using namespace util;

namespace tcp
{
namespace client
{

AprscClient::AprscClient(const std::string& cr_host,
        const std::string& cr_port, const std::string& cr_login, feed::Feed& r_feed)
        : Client(cr_host, cr_port, "(AprscClient)", r_feed),
          mLoginStr(cr_login),
          mStopped(false),
          mTimeout(mIoService, boost::posix_time::minutes(10))
{
    mLoginStr.append("\r\n");
    connect();
}

AprscClient::~AprscClient() noexcept
{
}

void AprscClient::connect() noexcept
{
    boost::asio::ip::tcp::resolver::query query(mHost, mPort,
            boost::asio::ip::tcp::resolver::query::canonical_name);
    mResolver.async_resolve(query,
            boost::bind(&AprscClient::handleResolve, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::iterator));
}

void AprscClient::process() noexcept
{
    mrFeed.process(mResponse);
}

void AprscClient::stop() noexcept
{
    Client::stop();
    mStopped = true;
    mTimeout.expires_at(boost::posix_time::pos_infin);
    mTimeout.cancel();
}

void AprscClient::handleResolve(const boost::system::error_code& cr_ec,
        boost::asio::ip::tcp::resolver::iterator it)
        noexcept
        {
    if (!cr_ec)
    {
        boost::asio::async_connect(mSocket, it,
                boost::bind(&AprscClient::handleConnect, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::iterator));
    } else
    {
        Logger::error("(AprscClient) resolve host: ", cr_ec.message());
        if (mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void AprscClient::handleConnect(const boost::system::error_code& cr_ec,
        boost::asio::ip::tcp::resolver::iterator it)
        noexcept
        {
    if (!cr_ec)
    {
        mSocket.set_option(boost::asio::socket_base::keep_alive(true));
        boost::asio::async_write(mSocket, boost::asio::buffer(mLoginStr),
                boost::bind(&AprscClient::handleLogin, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        mTimeout.async_wait(boost::bind(&AprscClient::sendKaBeacon, this));
    } else
    {
        Logger::error("(AprscClient) connect: ", cr_ec.message());
        if (mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void AprscClient::sendKaBeacon() noexcept
{
    if (mStopped)
    {
        return;
    }
    boost::asio::async_write(mSocket, boost::asio::buffer("#keep-alive beacon\r\n"),
            boost::bind(&AprscClient::handleSendKaBeacon, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    mTimeout.expires_from_now(boost::posix_time::minutes(10));
    mTimeout.async_wait(boost::bind(&AprscClient::sendKaBeacon, this));
}

void AprscClient::handleLogin(const boost::system::error_code& cr_ec, std::size_t s)
noexcept
{
    if (!cr_ec)
    {
        Logger::info("(AprscClient) connected to: ", mHost + ":" + mPort);
        read();
    } else
    {
        Logger::error("(AprscClient) send login: ", cr_ec.message());
    }
}

void AprscClient::handleSendKaBeacon(const boost::system::error_code& cr_ec,
        std::size_t s) noexcept
        {
    if (cr_ec)
    {
        Logger::error("(AprscClient) send beacon:", cr_ec.message());
    }
}

}  // namespace client
}  // namespace tcp
