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

#include "SBSClient.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <iostream>

#include "../../util/Logger.h"

SBSClient::SBSClient(boost::asio::signal_set& sigset, const std::string& host,
                     const std::string& port, Priority prio)
        : Client(sigset, host, port, "(SBSClient)", prio),
          mParser()
{
    connect();
}

SBSClient::~SBSClient() noexcept
{
}

void SBSClient::connect() noexcept
{
    boost::asio::ip::tcp::resolver::query query(
            mHost, mPort, boost::asio::ip::tcp::resolver::query::canonical_name);
    mResolver.async_resolve(
            query,
            boost::bind(&SBSClient::handleResolve, this, boost::asio::placeholders::error,
                        boost::asio::placeholders::iterator));
}

void SBSClient::process() noexcept
{
    mParser.unpack(mResponse, mPriority);
}

void SBSClient::handleResolve(const boost::system::error_code& ec,
                              boost::asio::ip::tcp::resolver::iterator it) noexcept
{
    if (!ec)
    {
        boost::asio::async_connect(
                mSocket,
                it,
                boost::bind(&SBSClient::handleConnect, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::iterator));
    }
    else
    {
        Logger::error("(SBSClient) resolve host: ", ec.message());
        if (mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void SBSClient::handleConnect(const boost::system::error_code& ec,
                              boost::asio::ip::tcp::resolver::iterator it) noexcept
{
    if (!ec)
    {
        mSocket.set_option(boost::asio::socket_base::keep_alive(true));
        Logger::info("(SBSClient) connected to: ", mHost);
        read();
    }
    else
    {
        Logger::error("(SBSClient) connect: ", ec.message());
        if (mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}
