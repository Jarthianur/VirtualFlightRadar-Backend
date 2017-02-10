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
#include <boost/system/error_code.hpp>
#include <csignal>
#include "../logger/Logger.h"

Client::Client(boost::asio::signal_set& s, const std::string& host,
        const std::string& port, const std::string& comp)
        : io_service_(),
          signals_(s),
          socket_(io_service_),
          resolver_(io_service_),
          host(host),
          port(port),
          component(comp),
          connect_timer(io_service_)

{
    awaitStop();
}

Client::~Client()
{
}

void Client::run()
{
    io_service_.run();
}

void Client::awaitStop()
{
    signals_.async_wait([this](const boost::system::error_code&, int)
    {
        stop();
    });
}

void Client::timedConnect()
{
    connect_timer.expires_from_now(boost::posix_time::seconds(C_CON_WAIT_TIMEVAL));
    connect_timer.async_wait(
            boost::bind(&Client::handleTimedConnect, this,
                        boost::asio::placeholders::error));
}

void Client::stop()
{
    Logger::info(component + " stop connection to: ", host);
    connect_timer.expires_at(boost::posix_time::pos_infin);
    connect_timer.cancel();
    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if (socket_.is_open())
    {
        socket_.close();
    }
}

void Client::read()
{
    boost::asio::async_read_until(
            socket_,
            buffer,
            "\n",
            boost::bind(&Client::handleRead, this, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
}

void Client::handleTimedConnect(const boost::system::error_code& ec)
{
    if (!ec)
    {
        Logger::info(component + " try connect to: ", host);
        connect();
    }
    else
    {
        Logger::error(component + " cancel connect: ", ec.message());
        if (ec != boost::asio::error::operation_aborted)
        {
            stop();
        }
    }
}

void Client::handleRead(const boost::system::error_code& ec, std::size_t s)
{
    if (!ec)
    {
        std::istream is(&buffer);
        std::getline(is, response);
        process();
        read();
    }
    else if (ec != boost::system::errc::bad_file_descriptor)
    {
        Logger::error(component + " read: ", ec.message());
        if (ec != boost::asio::error::operation_aborted)
        {
            stop();
        }
    }
}
