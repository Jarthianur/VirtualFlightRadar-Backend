/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

#include "Client.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time.hpp>
#include <iostream>

#include "../../util/Logger.h"

Client::Client(boost::asio::signal_set& sigset, const std::string& host,
               const std::string& port, const std::string& comp,
               std::unique_ptr<Parser> parser)
        : mIOservice(),
          mrSigSet(sigset),
          mSocket(mIOservice),
          mResolver(mIOservice),
          mHost(host),
          mPort(port),
          mComponent(comp),
          mParser(std::move(parser)),
          mConnectTimer(mIOservice)
{
    awaitStop();
}

Client::~Client() noexcept
{
}

void Client::run()
{
    mIOservice.run();
}

void Client::awaitStop()
{
    mrSigSet.async_wait([this](const boost::system::error_code&, int)
    {
        stop();
    });
}

void Client::timedConnect() noexcept
{
    mConnectTimer.expires_from_now(boost::posix_time::seconds(C_CON_WAIT_TIMEVAL));
    mConnectTimer.async_wait(
            boost::bind(&Client::handleTimedConnect, this,
                        boost::asio::placeholders::error));
}

void Client::stop() noexcept
{
    Logger::info(mComponent + " stop connection to: ", mHost);
    mConnectTimer.expires_at(boost::posix_time::pos_infin);
    mConnectTimer.cancel();
    boost::system::error_code ec;
    mSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if (mSocket.is_open())
    {
        mSocket.close();
    }
}

void Client::read() noexcept
{
    boost::asio::async_read_until(
            mSocket,
            mBuffer,
            "\n",
            boost::bind(&Client::handleRead, this, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
}

void Client::handleTimedConnect(const boost::system::error_code& ec) noexcept
{
    if (!ec)
    {
        Logger::info(mComponent + " try connect to: ", mHost);
        connect();
    }
    else
    {
        Logger::error(mComponent + " cancel connect: ", ec.message());
        if (ec != boost::asio::error::operation_aborted)
        {
            stop();
        }
    }
}

void Client::handleRead(const boost::system::error_code& ec, std::size_t s) noexcept
{
    if (!ec)
    {
        std::istream is(&mBuffer);
        std::getline(is, mResponse);
        process();
        read();
    }
    else if (ec != boost::system::errc::bad_file_descriptor)
    {
        Logger::error(mComponent + " read: ", ec.message());
        if (ec != boost::asio::error::operation_aborted)
        {
            stop();
        }
    }
}
