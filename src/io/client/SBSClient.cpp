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

#include "SBSClient.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <iostream>

#include "../logger/Logger.h"

SBSClient::SBSClient(boost::asio::signal_set& s, const std::string& host,
        const std::string& port)
        : Client(s, host, port, "(SBSClient)"),
          parser()
{
    connect();
}

SBSClient::~SBSClient() throw ()
{
}

void SBSClient::connect()
{
    boost::asio::ip::tcp::resolver::query query(
            host, port, boost::asio::ip::tcp::resolver::query::canonical_name);
    resolver_.async_resolve(
            query,
            boost::bind(&SBSClient::handleResolve, this, boost::asio::placeholders::error,
                        boost::asio::placeholders::iterator));
}

void SBSClient::process()
{
    parser.unpack(response);
}

void SBSClient::handleResolve(const boost::system::error_code& ec,
        boost::asio::ip::tcp::resolver::iterator it)
{
    if (!ec)
    {
        boost::asio::async_connect(
                socket_,
                it,
                boost::bind(&SBSClient::handleConnect, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::iterator));
    }
    else
    {
        Logger::error("(SBSClient) resolve host: ", ec.message());
        if (socket_.is_open())
        {
            socket_.close();
        }
        timedConnect();
    }
}

void SBSClient::handleConnect(const boost::system::error_code& ec,
        boost::asio::ip::tcp::resolver::iterator it)
{
    if (!ec)
    {
        socket_.set_option(boost::asio::socket_base::keep_alive(true));
        Logger::info("(SBSClient) connected to: ", host);
        read();
    }
    else
    {
        Logger::error("(SBSClient) connect: ", ec.message());
        if (socket_.is_open())
        {
            socket_.close();
        }
        timedConnect();
    }
}
