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

#include "SensorClient.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time.hpp>
#include <boost/operators.hpp>

#include "../../util/Logger.h"

SensorClient::SensorClient(boost::asio::signal_set& sigset, const std::string& host,
                       const std::string& port)
        : Client(sigset, host, port, "(WindClient)"),
          mStopped(false),
          mTimeout(mIOservice),
          mParser()
{
    connect();
    mTimeout.async_wait(boost::bind(&SensorClient::checkDeadline, this));
}

SensorClient::~SensorClient() noexcept
{
}

void SensorClient::read() noexcept
{
    mTimeout.expires_from_now(boost::posix_time::seconds(WC_RCV_TIMEOUT));
    Client::read();
}

void SensorClient::connect() noexcept
{
    boost::asio::ip::tcp::resolver::query query(
            mHost, mPort, boost::asio::ip::tcp::resolver::query::canonical_name);
    mResolver.async_resolve(
            query,
            boost::bind(&SensorClient::handleResolve, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::iterator));
}

void SensorClient::checkDeadline() noexcept
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
    mTimeout.async_wait(boost::bind(&SensorClient::checkDeadline, this));
}

void SensorClient::stop() noexcept
{
    Client::stop();
    mStopped = true;
    mTimeout.expires_at(boost::posix_time::pos_infin);
    mTimeout.cancel();
}

void SensorClient::process() noexcept
{
    mParser.unpack(mResponse);
}

void SensorClient::handleResolve(const boost::system::error_code& ec,
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
                boost::bind(&SensorClient::handleConnect, this,
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

void SensorClient::handleConnect(const boost::system::error_code& ec,
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
