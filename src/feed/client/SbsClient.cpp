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

#include "SbsClient.h"

#include <boost/bind.hpp>
#include <string>
#include <boost/thread/lock_guard.hpp>

#include "../../Logger.hpp"

#ifdef COMPONENT
#undef COMPONENT
#endif
#define COMPONENT "(SbsClient)"

namespace feed
{
namespace client
{
SbsClient::SbsClient(const Endpoint& crEndpoint) : Client(crEndpoint, COMPONENT)
{}

SbsClient::~SbsClient() noexcept
{}

void SbsClient::connect()
{
    mRunning = true;
    boost::asio::ip::tcp::resolver::query query(
        mEndpoint.host, mEndpoint.port, boost::asio::ip::tcp::resolver::query::canonical_name);
    mResolver.async_resolve(query, boost::bind(&SbsClient::handleResolve, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::iterator));
}

void SbsClient::handleResolve(const boost::system::error_code& crError,
                              boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept
{
    if(!crError)
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        boost::asio::async_connect(mSocket, vResolverIt,
                                   boost::bind(&SbsClient::handleConnect, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::iterator));
    }
    else if(crError != boost::asio::error::operation_aborted)
    {
        Logger::error(COMPONENT " resolve host: ", crError.message());
        boost::lock_guard<boost::mutex> lock(mMutex);
        if(mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void SbsClient::handleConnect(const boost::system::error_code& crError,
                              boost::asio::ip::tcp::resolver::iterator) noexcept
{
    if(!crError)
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        mSocket.set_option(boost::asio::socket_base::keep_alive(true));
        Logger::info(COMPONENT " connected to: ", mEndpoint.host, ":", mEndpoint.port);
        read();
    }
    else if(crError != boost::asio::error::operation_aborted)
    {
        Logger::error(COMPONENT " connect: ", crError.message());
        boost::lock_guard<boost::mutex> lock(mMutex);
        if(mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

}  // namespace client
}  // namespace feed
