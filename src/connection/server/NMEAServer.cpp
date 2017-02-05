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

#include "NMEAServer.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <algorithm>
#include <csignal>
#include <cstddef>
#include <iterator>

#include "../../util/Logger.h"

NMEAServer::NMEAServer(boost::asio::signal_set& s, in_port_t port)
        : io_service_(),
          signals_(s),
          acceptor_(io_service_,
                    boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port),
                    boost::asio::ip::tcp::acceptor::reuse_address(true)),
          socket_(io_service_)
{
    awaitStop();
    accept();
}

NMEAServer::~NMEAServer()
{
}

void NMEAServer::run()
{
    io_service_.run();
}

void NMEAServer::writeToAll(const std::string& msg)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    boost::system::error_code ec;
    for (auto it = clients.begin(); it != clients.end();)
    {
        boost::asio::write(it->get()->socket(), boost::asio::buffer(msg), ec);
        if (ec)
        {
            Logger::warn("(NMEAServer) lost connection to: ", it->get()->ip());
            clients.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void NMEAServer::accept()
{
    acceptor_.async_accept(
            socket_,
            boost::bind(&NMEAServer::handleAccept, this,
                        boost::asio::placeholders::error));
}

void NMEAServer::awaitStop()
{
    signals_.async_wait([this](const boost::system::error_code&, int)
    {
        acceptor_.close();
        stopAll();
    });
}

void NMEAServer::stopAll()
{
    Logger::info("(NMEAServer) stopping all clients...");
    clients.clear();
}

void NMEAServer::handleAccept(const boost::system::error_code& ec)
{
    if (!acceptor_.is_open())
    {
        return;
    }
    if (!ec)
    {
        auto client = Connection::start(std::move(socket_));
        if (clients.size() < MAX_CLIENTS)
        {
            clients.push_back(client);
            Logger::info("(NMEAServer) connection from: ", client->ip());
        }
        else
        {
            Logger::info("(NMEAServer) client count exceeded, refuse: ", client->ip());
        }
    }
    else if (ec != boost::system::errc::bad_file_descriptor)
    {
        Logger::warn("(NMEAServer) accept: ", ec.message());
    }
    accept();
}
