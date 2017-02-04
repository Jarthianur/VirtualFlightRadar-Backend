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
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <iostream>

#include "../../util/Logger.h"

WindClient::WindClient(boost::asio::signal_set& s, const std::string& host,
        const std::string& port)
        : Client(s, host, port),
          stopped_(false),
          deadline_(io_service_),
          parser()
{
    connect();
    deadline_.async_wait(boost::bind(&WindClient::checkDeadline, this));
}

WindClient::~WindClient() throw ()
{
}

void WindClient::read()
{
    deadline_.expires_from_now(boost::posix_time::seconds(WC_TIMEOUT));
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
                    Logger::error("(WindClient) read error: ", ec.message());
                }
            });
}

void WindClient::connect()
{
    boost::asio::ip::tcp::resolver::query query(
            host, port, boost::asio::ip::tcp::resolver::query::canonical_name);
    resolver_.async_resolve(
            query,
            [this](const boost::system::error_code& ec,
                    boost::asio::ip::tcp::resolver::iterator it)
            {
                if (stopped_)
                {
                    return;
                }
                if (!ec)
                {
                    boost::asio::async_connect(socket_, it,
                            [this](const boost::system::error_code ec,
                                    boost::asio::ip::tcp::resolver::iterator)
                            {
                                if (stopped_)
                                {
                                    return;
                                }
                                if (!ec)
                                {
                                    socket_.set_option(boost::asio::socket_base::keep_alive(true)); // necessary ?
                                    read();
                                }
                                else
                                {
                                    Logger::error("(WindClient) failed to connect host: ", ec.message());
                                    reconnect();
                                }
                            });
                }
                else
                {
                    Logger::error("(WindClient) failed to resolve host: ", ec.message());
                    reconnect();
                }
            });
}

void WindClient::checkDeadline()
{
    if (stopped_)
    {
        return;
    }
    if (deadline_.expires_at() <= boost::asio::deadline_timer::traits_type::now())
    {
        socket_.close();
        deadline_.expires_at(boost::posix_time::pos_infin);
        connect();
        Logger::warn("(WindClient) timed out: reconnect...");
    }
    deadline_.async_wait(boost::bind(&WindClient::checkDeadline, this));
}

void WindClient::stop()
{
    stopped_ = true;
}

void WindClient::awaitStop()
{
    signals_.async_wait([this](const boost::system::error_code&, int)
    {
        Logger::info("(WindClient) stopping...");
        socket_.close();
        stop();
    });
}
