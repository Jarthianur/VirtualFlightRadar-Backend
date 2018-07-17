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

#include "SensorClient.h"

#include <string>
#include <boost/bind.hpp>
#include <boost/date_time.hpp>
#include <boost/operators.hpp>
#include <boost/thread/lock_guard.hpp>

#include "../../Logger.hpp"

#ifdef COMPONENT
#undef COMPONENT
#endif
#define COMPONENT "(SensorClient)"

namespace feed
{
namespace client
{
SensorClient::SensorClient(const Endpoint& crEndpoint)
    : Client(crEndpoint, COMPONENT), mTimeout(mIoService)
{}

SensorClient::~SensorClient() noexcept
{}

void SensorClient::read()
{
    if(mRunning)
    {
        mTimeout.expires_from_now(boost::posix_time::seconds(WC_RCV_TIMEOUT));
    }
    Client::read();
}

void SensorClient::connect()
{
    mRunning = true;
    mTimeout.async_wait(boost::bind(&SensorClient::checkDeadline, this));
    boost::asio::ip::tcp::resolver::query query(
        mEndpoint.host, mEndpoint.port, boost::asio::ip::tcp::resolver::query::canonical_name);
    mResolver.async_resolve(query, boost::bind(&SensorClient::handleResolve, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::iterator));
}

void SensorClient::checkDeadline()
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    if(mRunning)
    {
        if(mTimeout.expires_at() <= boost::asio::deadline_timer::traits_type::now())
        {
            logger.warn(COMPONENT " timed out: reconnect...");
            stop();
            connect();
        }
        mTimeout.async_wait(boost::bind(&SensorClient::checkDeadline, this));
    }
}

void SensorClient::stop()
{
    if(mRunning)
    {
        mTimeout.expires_at(boost::posix_time::pos_infin);
        mTimeout.cancel();
    }
    Client::stop();
}

void SensorClient::handleResolve(const boost::system::error_code& crError,
                                 boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept
{
    if(!crError)
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        boost::asio::async_connect(mSocket, vResolverIt,
                                   boost::bind(&SensorClient::handleConnect, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::iterator));
    }
    else if(crError != boost::asio::error::operation_aborted)
    {
        logger.error(COMPONENT " resolve host: ", crError.message());
        boost::lock_guard<boost::mutex> lock(mMutex);
        closeSocket();
        timedConnect();
    }
}

void SensorClient::handleConnect(const boost::system::error_code& crError,
                                 boost::asio::ip::tcp::resolver::iterator) noexcept
{
    if(!crError)
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        mSocket.set_option(boost::asio::socket_base::keep_alive(true));
        logger.info(COMPONENT " connected to: ", mEndpoint.host, ":", mEndpoint.port);
        read();
    }
    else if(crError != boost::asio::error::operation_aborted)
    {
        logger.error(COMPONENT " connect: ", crError.message());
        boost::lock_guard<boost::mutex> lock(mMutex);
        closeSocket();
        timedConnect();
    }
}

}  // namespace client
}  // namespace feed
