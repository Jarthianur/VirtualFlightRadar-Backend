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
#include <cstddef>
#include <iostream>
#include "../../vfrb/Feed.h"
#include "../../util/Logger.h"

using namespace util;

namespace tcp
{
namespace client
{

AprscClient::AprscClient(boost::asio::signal_set& r_sigset,
        const std::string& cr_host, const std::string& cr_port,
        const std::string& cr_login, vfrb::Feed& r_feed)
        : Client(r_sigset, cr_host, cr_port, "(APRSCClient)", r_feed),
          mLoginStr(cr_login)
{
    mLoginStr.append("\r\n");
    connect();
}

AprscClient::~AprscClient() noexcept
{
}

void AprscClient::connect() noexcept
{
    boost::asio::ip::tcp::resolver::query query(
            mHost, mPort,
            boost::asio::ip::tcp::resolver::query::canonical_name);
    mResolver.async_resolve(
            query,
            boost::bind(&AprscClient::handleResolve, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::iterator));
}

void AprscClient::process() noexcept
{
    mrFeed.process(mResponse);
}

void AprscClient::handleResolve(const boost::system::error_code& cr_ec,
        boost::asio::ip::tcp::resolver::iterator it)
        noexcept
        {
    if (!cr_ec)
    {
        boost::asio::async_connect(
                mSocket,
                it,
                boost::bind(&AprscClient::handleConnect, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::iterator));
    } else
    {
        Logger::error("(APRSCClient) resolve host: ", cr_ec.message());
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
        boost::asio::async_write(
                mSocket,
                boost::asio::buffer(mLoginStr),
                boost::bind(&AprscClient::handleLogin, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
    } else
    {
        Logger::error("(APRSCClient) connect: ", cr_ec.message());
        if (mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void AprscClient::handleLogin(const boost::system::error_code& cr_ec,
        std::size_t s) noexcept
        {
    if (!cr_ec)
    {
        Logger::info("(APRSCClient) connected to: ", mHost + ":" + mPort);
        read();
    } else
    {
        Logger::error("(APRSCClient) send login: ", cr_ec.message());
    }
}

}  // namespace client
}  // namespace tcp
