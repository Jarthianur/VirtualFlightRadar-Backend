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

#include "../Logger.h"

namespace server
{
Server::Server(std::uint16_t port)
    : mIOservice(),
      mAcceptor(mIOservice,
                boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port),
                boost::asio::ip::tcp::acceptor::reuse_address(true)),
      mSocket(mIOservice)
{}

Server::~Server() noexcept
{}

void Server::run(boost::asio::signal_set& r_sigset)
{
    awaitStop(r_sigset);
    accept();
    mIOservice.run();
}

void Server::writeToAll(const std::string& cr_msg)
{
    if(cr_msg.empty())
    {
        return;
    }
    boost::lock_guard<boost::mutex> lock(this->mMutex);
    boost::system::error_code ec;
    for(auto it = mClients.begin(); it != mClients.end();)
    {
        boost::asio::write(it->get()->getSocket(), boost::asio::buffer(cr_msg), ec);
        if(ec)
        {
            Logger::warn({"(Server) lost connection to: ", it->get()->getIp()});
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
    mAcceptor.async_accept(mSocket, boost::bind(&Server::handleAccept, this,
                                                boost::asio::placeholders::error));
}

void Server::awaitStop(boost::asio::signal_set& r_sigset)
{
    r_sigset.async_wait([this](const boost::system::error_code&, int) {
        mAcceptor.close();
        stopAll();
    });
}

void Server::stopAll()
{
    boost::lock_guard<boost::mutex> lock(this->mMutex);
    Logger::info({"(Server) stopping all clients..."});
    mClients.clear();
}

bool Server::isConnected(const std::string& cr_ip)
{
    for(auto it = mClients.cbegin(); it != mClients.cend(); it++)
    {
        if(it->get()->getIp() == cr_ip)
        {
            return true;
        }
    }
    return false;
}

void Server::handleAccept(const boost::system::error_code& cr_ec) noexcept
{
    if(!mAcceptor.is_open())
    {
        return;
    }
    if(!cr_ec)
    {
        boost::lock_guard<boost::mutex> lock(this->mMutex);
        auto client = Connection::start(boost::move(mSocket));
        if(mClients.size() < S_MAX_CLIENTS && !isConnected(client->getIp()))
        {
            mClients.push_back(client);
            Logger::info({"(Server) connection from: ", client->getIp()});
        }
        else
        {
            Logger::info({"(Server) refused connection to ", client->getIp()});
        }
    }
    else if(cr_ec != boost::system::errc::bad_file_descriptor)
    {
        Logger::warn({"(Server) accept: ", cr_ec.message()});
    }
    accept();
}

}  // namespace server
