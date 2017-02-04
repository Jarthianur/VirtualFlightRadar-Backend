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
#include <boost/system/error_code.hpp>
#include <csignal>
#include "../../util/Logger.h"

Client::Client(boost::asio::signal_set& s, const std::string& host,
        const std::string& port)
        : io_service_(),
          signals_(s),
          socket_(io_service_),
          resolver_(io_service_),
          host(host),
          port(port),
          deadline_(io_service_)

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
        Logger::info("(Client) stopping...");
        socket_.close();
    });
}

void Client::reconnect()
{
    socket_.close();
    deadline_.expires_from_now(boost::posix_time::seconds(WAIT_TIMEVAL));
    deadline_.async_wait([this](const boost::system::error_code& ec)
    {
        if (!ec)
        {
            Logger::info("(Client) try to reconnect...");
            connect();
        }
        else
        {
            Logger::error("(Client) error while waiting: ", ec.message());
        }
    });
}
