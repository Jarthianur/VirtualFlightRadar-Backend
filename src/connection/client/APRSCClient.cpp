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
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <iostream>

#include "../../util/Logger.h"

APRSCClient::APRSCClient(boost::asio::signal_set& s, const std::string& host,
        const std::string& port, const std::string& login)
        : Client(s, host, port),
          login_str(login),
          parser()
{
    login_str.append("\r\n");
    connect();
}

APRSCClient::~APRSCClient()
{
}

void APRSCClient::read()
{
    boost::asio::async_read_until(
            socket_, buffer, "\r\n",
            [this](const boost::system::error_code& ec, std::size_t)
            {
                if (!ec)
                {
                    std::istream is(&buffer);
                    std::getline(is, response);
                    parser.unpack(response);
                    read();
                }
                else if (ec != boost::system::errc::bad_file_descriptor &&
                        ec != boost::system::errc::operation_canceled)
                {
                    Logger::error("(APRSCClient) read error: ", ec.message());
                }
            });
}

void APRSCClient::connect()
{
    boost::asio::ip::tcp::resolver::query query(
            host, port, boost::asio::ip::tcp::resolver::query::canonical_name);
    resolver_.async_resolve(
            query,
            [this](const boost::system::error_code& ec,
                    boost::asio::ip::tcp::resolver::iterator it)
            {
                if (!ec)
                {
                    boost::asio::async_connect(socket_, it,
                            [this](const boost::system::error_code& ec,
                                    boost::asio::ip::tcp::resolver::iterator)
                            {
                                if (!ec)
                                {
                                    socket_.set_option(boost::asio::socket_base::keep_alive(true));
                                    boost::asio::async_write(socket_,
                                            boost::asio::buffer(login_str),
                                            [this](const boost::system::error_code& ec, std::size_t)
                                            {
                                                if (!ec)
                                                {
                                                    read();
                                                }
                                                else
                                                {
                                                    Logger::error("(SBSClient) failed to send login: ", ec.message());
                                                }
                                            });
                                }
                                else
                                {
                                    Logger::error("(SBSClient) failed to connect host: ", ec.message());
                                    reconnect();
                                }
                            });
                }
                else
                {
                    Logger::error("(SBSClient) failed to resolve host: ", ec.message());
                    reconnect();
                }
            });
}
