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

#include "Client.h"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/date_time.hpp>
#include <boost/functional/hash.hpp>
#include <boost/thread/lock_guard.hpp>

#include "../../Logger.hpp"
#include "../Feed.h"

namespace feed
{
namespace client
{
Client::Client(const Endpoint& crEndpoint, const std::string& crComponent)
    : mIoService(),
      mSocket(mIoService),
      mResolver(mIoService),
      mEndpoint(crEndpoint),
      mComponent(crComponent),
      mConnectTimer(mIoService)
{
    Logger::debug("Client construct ", mComponent);
}

Client::~Client() noexcept
{
    Logger::debug("Client destruct ", mComponent);
}

void Client::run()
{
    Logger::debug("Client run called ", mComponent);
    if(!mRunning)
    {
        Logger::debug("Client is running ", mComponent);
        {
            boost::lock_guard<boost::mutex> lock(mMutex);
            connect();
        }
        mIoService.run();
    }
}

bool Client::equals(const Client& crOther) const
{
    return this->mEndpoint == crOther.mEndpoint;
}

std::size_t Client::hash() const
{
    std::size_t seed = 0;
    boost::hash_combine(seed, boost::hash_value(mEndpoint.host));
    boost::hash_combine(seed, boost::hash_value(mEndpoint.port));
    return seed;
}

void Client::subscribe(std::shared_ptr<Feed>& rpFeed)
{
    Logger::debug(mComponent, " Client subscribed from ", rpFeed->getName());
    boost::lock_guard<boost::mutex> lock(mMutex);
    mrFeeds.push_back(rpFeed);
}

void Client::timedConnect()
{
    Logger::debug("Client timedConnect called ", mComponent);
    Logger::debug("Client is running ", mComponent);
    mConnectTimer.expires_from_now(boost::posix_time::seconds(C_CON_WAIT_TIMEVAL));
    mConnectTimer.async_wait(
        boost::bind(&Client::handleTimedConnect, this, boost::asio::placeholders::error));
}

void Client::stop()
{
    Logger::debug("Client stop called ", mComponent);
    if(mRunning)
    {
        Logger::debug("Client is running ", mComponent);
        mRunning = false;
        Logger::info(mComponent, " stop connection to: ", mEndpoint.host, ":", mEndpoint.port);
        mConnectTimer.expires_at(boost::posix_time::pos_infin);
        mConnectTimer.cancel();
        boost::system::error_code ec;
        mSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        if(mSocket.is_open())
        {
            mSocket.close();
        }
    }
}

void Client::lockAndStop()
{
    Logger::debug(mComponent, " Client lockandstop called");
    boost::lock_guard<boost::mutex> lock(mMutex);
    stop();
}

void Client::read()
{
    Logger::debug("Client read called ", mComponent);
    if(mRunning)
    {
        Logger::debug("Client is running ", mComponent);
        boost::asio::async_read_until(mSocket, mBuffer, "\r\n",
                                      boost::bind(&Client::handleRead, this,
                                                  boost::asio::placeholders::error,
                                                  boost::asio::placeholders::bytes_transferred));
    }
}

void Client::handleTimedConnect(const boost::system::error_code& crError) noexcept
{
    if(!crError)
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        Logger::info(mComponent, " try connect to: ", mEndpoint.host, ":", mEndpoint.port);
        connect();
    }
    else
    {
        Logger::error(mComponent, " cancel connect: ", crError.message());
        if(crError != boost::asio::error::operation_aborted)
        {
            boost::lock_guard<boost::mutex> lock(mMutex);
            stop();
        }
    }
}

void Client::handleRead(const boost::system::error_code& crError, std::size_t) noexcept
{
    if(!crError)
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        std::istream is(&mBuffer);
        std::getline(is, mResponse);
        mResponse.append("\n");
        for(auto& it : mrFeeds)
        {
            if(!mRunning)
            {
                break;
            }
            it->process(mResponse);
        }
        read();
    }
    else if(crError != boost::system::errc::bad_file_descriptor)
    {
        Logger::error(mComponent, " read: ", crError.message());
        if(crError != boost::asio::error::operation_aborted)
        {
            boost::lock_guard<boost::mutex> lock(mMutex);
            stop();
            if(crError == boost::asio::error::eof)
            {
                timedConnect();
            }
        }
    }
}

}  // namespace client
}  // namespace feed
