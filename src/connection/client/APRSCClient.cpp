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

#include "APRSCClient.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <iostream>

#include "../../util/Logger.h"

APRSCClient::APRSCClient(boost::asio::signal_set& s, const std::string& host,
        const std::string& port, const std::string& login)
        : Client(s, host, port, "(APRSCClient)"),
          login_str(login),
          parser()
{
    login_str.append("\r\n");
    connect();
}

APRSCClient::~APRSCClient()
{
}

void APRSCClient::connect()
{
    boost::asio::ip::tcp::resolver::query query(
            host, port, boost::asio::ip::tcp::resolver::query::canonical_name);
    resolver_.async_resolve(
            query,
            boost::bind(&APRSCClient::handleResolve, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::iterator));
}

void APRSCClient::process()
{
    parser.unpack(response);
}

void APRSCClient::handleResolve(const boost::system::error_code& ec,
        boost::asio::ip::tcp::resolver::iterator it)
{
    if (!ec)
    {
        boost::asio::async_connect(
                socket_,
                it,
                boost::bind(&APRSCClient::handleConnect, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::iterator));
    }
    else
    {
        Logger::error("(APRSCClient) resolve host: ", ec.message());
        socket_.close();
        timedConnect();
    }
}

void APRSCClient::handleConnect(const boost::system::error_code& ec,
        boost::asio::ip::tcp::resolver::iterator it)
{
    if (!ec)
    {
        socket_.set_option(boost::asio::socket_base::keep_alive(true));
        boost::asio::async_write(
                socket_,
                boost::asio::buffer(login_str),
                boost::bind(&APRSCClient::handleLogin, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        Logger::error("(APRSCClient) connect: ", ec.message());
        socket_.close();
        timedConnect();
    }
}

void APRSCClient::handleLogin(const boost::system::error_code& ec, std::size_t s)
{
    if (!ec)
    {
        read();
    }
    else
    {
        Logger::error("(APRSCClient) send login: ", ec.message());
    }
}
