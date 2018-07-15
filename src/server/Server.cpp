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
    boost::system::error_code ec;
    auto buffer = boost::asio::buffer(crStr);
    boost::lock_guard<boost::mutex> lock(mMutex);
    for(auto& it : mConnections)
    {
        if(it)
        {
            boost::asio::write(it.get()->getSocket(), buffer, ec);
            if(ec)
            {
                Logger::warn("(Server) lost connection to: ", it.get()->getIpAddress());
                it.reset();
            }
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
        boost::lock_guard<boost::mutex> lock(mMutex);
        mAcceptor.close();
        stop();
    });
}

void Server::stop()
{
    Logger::info("(Server) stopping all connections...");
    for(auto& it : mConnections)
    {
        if(it)
        {
            it.reset();
        }
    }
}

bool Server::isConnected(const std::string& crIpAddress)
{
    for(const auto& it : mConnections)
    {
        if(it && it.get()->getIpAddress() == crIpAddress)
        {
            return true;
        }
    }
    return false;
}

void Server::handleAccept(const boost::system::error_code& crError) noexcept
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    if(!mAcceptor.is_open())
    {
        return;
    }
    if(!crError)
    {
        bool success = false;
        if(!isConnected(mSocket.remote_endpoint().address().to_string()))
        {
            for(auto& it : mConnections)
            {
                if(!it)
                {
                    success = true;
                    it      = Connection::start(boost::move(mSocket));
                    Logger::info("(Server) connection from: ", it->getIpAddress());
                    break;
                }
            }
        }
        if(!success)
        {
            Logger::info("(Server) refused connection to ",
                         mSocket.remote_endpoint().address().to_string());
            boost::system::error_code ec;
            mSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
            mSocket.close();
        }
    }
    else if(crError != boost::system::errc::bad_file_descriptor)
    {
        Logger::warn("(Server) accept: ", crError.message());
    }
    accept();
}

}  // namespace server
