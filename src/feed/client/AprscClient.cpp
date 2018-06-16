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

#include "AprscClient.h"

#include <boost/bind.hpp>
#include <boost/date_time.hpp>
#include <boost/functional/hash.hpp>

#include "../../Logger.hpp"

#ifdef COMPONENT
#undef COMPONENT
#endif
#define COMPONENT "(AprscClient)"

namespace feed
{
namespace client
{
AprscClient::AprscClient(const Endpoint& crEndpoint, const std::string& crLogin)
    : Client(crEndpoint, COMPONENT),
      mLoginStr(crLogin),
      mTimeout(mIoService, boost::posix_time::minutes(10))
{
    mLoginStr.append("\r\n");
}

AprscClient::~AprscClient() noexcept
{}

bool AprscClient::equals(const Client& crOther) const
{
    try
    {
        const AprscClient& crAOther = dynamic_cast<const AprscClient&>(crOther);
        return Client::equals(crOther) && this->mLoginStr == crAOther.mLoginStr;
    }
    catch(const std::bad_cast&)
    {
        return false;
    }
}

std::size_t AprscClient::hash() const
{
    std::size_t seed = Client::hash();
    boost::hash_combine(seed, boost::hash_value(mLoginStr));
    return seed;
}

void AprscClient::connect()
{
    boost::asio::ip::tcp::resolver::query query(
        mEndpoint.host, mEndpoint.port,
        boost::asio::ip::tcp::resolver::query::canonical_name);
    mResolver.async_resolve(query, boost::bind(&AprscClient::handleResolve, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::iterator));
}

void AprscClient::stop()
{
    Client::stop();
    mTimeout.expires_at(boost::posix_time::pos_infin);
    mTimeout.cancel();
}

void AprscClient::handleResolve(
    const boost::system::error_code& crError,
    boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept
{
    if(!crError)
    {
        boost::asio::async_connect(mSocket, vResolverIt,
                                   boost::bind(&AprscClient::handleConnect, this,
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

void AprscClient::handleConnect(const boost::system::error_code& crError,
                                boost::asio::ip::tcp::resolver::iterator) noexcept
{
    if(!crError)
    {
        mSocket.set_option(boost::asio::socket_base::keep_alive(true));
        boost::asio::async_write(
            mSocket, boost::asio::buffer(mLoginStr),
            boost::bind(&AprscClient::handleLogin, this, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        mTimeout.async_wait(boost::bind(&AprscClient::sendKeepAlive, this));
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

void AprscClient::sendKeepAlive()
{
    if(!mRunning)
    {
        return;
    }
    boost::asio::async_write(mSocket, boost::asio::buffer("#keep-alive beacon\r\n"),
                             boost::bind(&AprscClient::handleSendKeepAlive, this,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
    mTimeout.expires_from_now(boost::posix_time::minutes(10));
    mTimeout.async_wait(boost::bind(&AprscClient::sendKeepAlive, this));
}

void AprscClient::handleLogin(const boost::system::error_code& crError,
                              std::size_t) noexcept
{
    if(!crError)
    {
        Logger::info(COMPONENT " connected to: ", mEndpoint.host, ":", mEndpoint.port);
        read();
    }
    else
    {
        Logger::error(COMPONENT " send login: ", crError.message());
    }
}

void AprscClient::handleSendKeepAlive(const boost::system::error_code& crError,
                                      std::size_t) noexcept
{
    if(crError)
    {
        Logger::error(COMPONENT " send beacon:", crError.message());
    }
}

}  // namespace client
}  // namespace feed
