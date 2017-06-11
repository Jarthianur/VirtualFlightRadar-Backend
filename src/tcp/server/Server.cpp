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

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <algorithm>
#include <iterator>
#include <boost/thread/lock_guard.hpp>

#include "../../util/Logger.h"

using namespace util;

namespace tcp
{
namespace server
{

Server::Server(boost::asio::signal_set& r_sigset, std::uint16_t port)
        : mIOservice(),
          mrSigSet(r_sigset),
          mAcceptor(mIOservice,
                  boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
                          port),
                  boost::asio::ip::tcp::acceptor::reuse_address(true)),
          mSocket(mIOservice)
{
    awaitStop();
    accept();
}

Server::~Server() noexcept
{
}

void Server::run()
{
    mIOservice.run();
}

void Server::writeToAll(const std::string& cr_msg) noexcept
{
    boost::lock_guard<boost::mutex> lock(this->mMutex);
    boost::system::error_code ec;
    for (auto it = mClients.begin(); it != mClients.end();)
    {
        boost::asio::write(it->get()->getSocket(), boost::asio::buffer(cr_msg),
                ec);
        if (ec)
        {
            Logger::warn("(Server) lost connection to: ",
                    it->get()->getIP());
            mClients.erase(it);
        } else
        {
            ++it;
        }
    }
}

void Server::accept() noexcept
{
    mAcceptor.async_accept(mSocket,
            boost::bind(&Server::handleAccept, this,
                    boost::asio::placeholders::error));
}

void Server::awaitStop()
{
    mrSigSet.async_wait([this](const boost::system::error_code&, int)
    {
        mAcceptor.close();
        stopAll();
    });
}

void Server::stopAll()
{
    boost::lock_guard<boost::mutex> lock(this->mMutex);
    Logger::info("(Server) stopping all clients...");
    mClients.clear();
}

void Server::handleAccept(const boost::system::error_code& cr_ec) noexcept
{
    if (!mAcceptor.is_open())
    {
        return;
    }
    if (!cr_ec)
    {
        auto client = Connection::start(std::move(mSocket));
        if (mClients.size() < S_MAX_CLIENTS)
        {
            mClients.push_back(client);
            Logger::info("(Server) connection from: ", client->getIP());
        } else
        {
            Logger::info("(Server) client count exceeded, refuse: ",
                    client->getIP());
        }
    } else if (cr_ec != boost::system::errc::bad_file_descriptor)
    {
        Logger::warn("(Server) accept: ", cr_ec.message());
    }
    accept();
}

}  // namespace server
}  // namespace tcp
