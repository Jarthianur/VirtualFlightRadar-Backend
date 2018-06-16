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

#include <boost/bind.hpp>
#include <boost/date_time.hpp>
#include <boost/operators.hpp>

#include "../../Logger.hpp"

#ifdef COMPONENT
#undef COMPONENT
#endif
#define COMPONENT "(SensorClient)"

namespace feed
{
namespace client
{
SensorClient::SensorClient(const std::string& crHost, const std::string& crPort)
    : Client({crHost, crPort}, COMPONENT), mTimeout(mIoService)
{}

SensorClient::~SensorClient() noexcept
{}

void SensorClient::read()
{
    mTimeout.expires_from_now(boost::posix_time::seconds(WC_RCV_TIMEOUT));
    Client::read();
}

void SensorClient::connect()
{
    mTimeout.async_wait(boost::bind(&SensorClient::checkDeadline, this));
    boost::asio::ip::tcp::resolver::query query(
        mEndpoint.host, mEndpoint.port,
        boost::asio::ip::tcp::resolver::query::canonical_name);
    mResolver.async_resolve(query, boost::bind(&SensorClient::handleResolve, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::iterator));
}

void SensorClient::checkDeadline()
{
    if(!mRunning)
    {
        return;
    }
    if(mTimeout.expires_at() <= boost::asio::deadline_timer::traits_type::now())
    {
        Logger::warn(COMPONENT " timed out: reconnect...");
        if(mSocket.is_open())
        {
            mSocket.close();
        }
        mTimeout.expires_at(boost::posix_time::pos_infin);
        connect();
    }
    mTimeout.async_wait(boost::bind(&SensorClient::checkDeadline, this));
}

void SensorClient::stop()
{
    Client::stop();
    mTimeout.expires_at(boost::posix_time::pos_infin);
    mTimeout.cancel();
}

void SensorClient::handleResolve(
    const boost::system::error_code& crError,
    boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept
{
    if(!crError)
    {
        boost::asio::async_connect(mSocket, vResolverIt,
                                   boost::bind(&SensorClient::handleConnect, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::iterator));
    }
    else
    {
        Logger::error(COMPONENT " resolve host: ", crError.message());
        if(mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void SensorClient::handleConnect(const boost::system::error_code& crError,
                                 boost::asio::ip::tcp::resolver::iterator) noexcept
{
    if(!crError)
    {
        mSocket.set_option(boost::asio::socket_base::keep_alive(true));
        Logger::info(COMPONENT " connected to: ", mEndpoint.host, ":", mEndpoint.port);
        read();
    }
    else
    {
        Logger::error(COMPONENT " connect: ", crError.message());
        if(mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

}  // namespace client
}  // namespace feed
