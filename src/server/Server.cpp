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

#include "Server.h"

#include <algorithm>
#include <iterator>
#include <boost/bind.hpp>
#include <boost/move/move.hpp>
#include <boost/thread/lock_guard.hpp>

#include "../Logger.hpp"

namespace server
{
Server::Server() : Server(4353)
{}

Server::Server(std::uint16_t vPort)
    : mIoService(),
      mAcceptor(mIoService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), vPort),
                boost::asio::ip::tcp::acceptor::reuse_address(true)),
      mSocket(mIoService)
{}

Server::~Server() noexcept
{}

void Server::run(boost::asio::signal_set& rSigset)
{
    awaitStop(rSigset);
    accept();
    mIoService.run();
}

void Server::send(const std::string& crStr)
{
    if(crStr.empty())
    {
        return;
    }
    boost::lock_guard<boost::mutex> lock(mMutex);
    boost::system::error_code ec;
    for(auto it = mClients.begin(); it != mClients.end();)
    {
        boost::asio::write(it->get()->getSocket(), boost::asio::buffer(crStr), ec);
        if(ec)
        {
            Logger::warn("(Server) lost connection to: ", it->get()->getIpAddress());
            mClients.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Server::accept()
{
    mAcceptor.async_accept(
        mSocket, boost::bind(&Server::handleAccept, this, boost::asio::placeholders::error));
}

void Server::awaitStop(boost::asio::signal_set& rSigset)
{
    rSigset.async_wait([this](const boost::system::error_code&, int) {
        mAcceptor.close();
        stop();
    });
}

void Server::stop()
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    Logger::info("(Server) stopping all clients...");
    mClients.clear();
}

bool Server::isConnected(const std::string& crIpAddress)
{
    for(auto it = mClients.cbegin(); it != mClients.cend(); it++)
    {
        if(it->get()->getIpAddress() == crIpAddress)
        {
            return true;
        }
    }
    return false;
}

void Server::handleAccept(const boost::system::error_code& crError) noexcept
{
    if(!mAcceptor.is_open())
    {
        return;
    }
    if(!crError)
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        auto client = Connection::start(boost::move(mSocket));
        if(mClients.size() < S_MAX_CLIENTS && !isConnected(client->getIpAddress()))
        {
            mClients.push_back(client);
            Logger::info("(Server) connection from: ", client->getIpAddress());
        }
        else
        {
            Logger::info("(Server) refused connection to ", client->getIpAddress());
        }
    }
    else if(crError != boost::system::errc::bad_file_descriptor)
    {
        Logger::warn("(Server) accept: ", crError.message());
    }
    accept();
}

}  // namespace server
