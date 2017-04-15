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

#include "WindClient.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time.hpp>
#include <boost/operators.hpp>

#include "../../parser/WindParser.h"
#include "../../util/Logger.h"

WindClient::WindClient(boost::asio::signal_set& sigset, const std::string& host,
                       const std::string& port)
        : Client(sigset, host, port, "(WindClient)",
                 std::unique_ptr<Parser>(new WindParser())),
          mStopped(false),
          mTimeout(mIOservice)
{
    connect();
    mTimeout.async_wait(boost::bind(&WindClient::checkDeadline, this));
}

WindClient::~WindClient() noexcept
{
}

void WindClient::read() noexcept
{
    mTimeout.expires_from_now(boost::posix_time::seconds(WC_RCV_TIMEOUT));
    Client::read();
}

void WindClient::connect() noexcept
{
    boost::asio::ip::tcp::resolver::query query(
            mHost, mPort, boost::asio::ip::tcp::resolver::query::canonical_name);
    mResolver.async_resolve(
            query,
            boost::bind(&WindClient::handleResolve, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::iterator));
}

void WindClient::checkDeadline() noexcept
{
    if (mStopped)
    {
        return;
    }
    if (mTimeout.expires_at() <= boost::asio::deadline_timer::traits_type::now())
    {
        Logger::warn("(WindClient) timed out: reconnect...");
        if (mSocket.is_open())
        {
            mSocket.close();
        }
        mTimeout.expires_at(boost::posix_time::pos_infin);
        connect();
    }
    mTimeout.async_wait(boost::bind(&WindClient::checkDeadline, this));
}

void WindClient::stop() noexcept
{
    Client::stop();
    mStopped = true;
    mTimeout.expires_at(boost::posix_time::pos_infin);
    mTimeout.cancel();
}

void WindClient::process() noexcept
{
    mParser->unpack(mResponse);
}

void WindClient::handleResolve(const boost::system::error_code& ec,
                               boost::asio::ip::tcp::resolver::iterator it) noexcept
{
    if (mStopped)
    {
        return;
    }
    if (!ec)
    {
        boost::asio::async_connect(
                mSocket,
                it,
                boost::bind(&WindClient::handleConnect, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::iterator));
    }
    else
    {
        Logger::error("(WindClient) resolve host: ", ec.message());
        if (mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void WindClient::handleConnect(const boost::system::error_code& ec,
                               boost::asio::ip::tcp::resolver::iterator it) noexcept
{
    if (mStopped)
    {
        return;
    }
    if (!ec)
    {
        mSocket.set_option(boost::asio::socket_base::keep_alive(true));
        Logger::info("(WindClient) connected to: ", mHost);
        read();
    }
    else
    {
        Logger::error("(WindClient) connect: ", ec.message());
        if (mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}
