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

#include "../../Logger.hpp"

namespace feed
{
namespace client
{
SbsClient::SbsClient(const std::string& cr_host, const std::string& cr_port,
                     feed::Feed& r_feed)
    : Client(cr_host, cr_port, "(SbsClient)", r_feed)
{
    connect();
}

SbsClient::~SbsClient() noexcept
{}

void SbsClient::connect()
{
    boost::asio::ip::tcp::resolver::query query(
        mHost, mPort, boost::asio::ip::tcp::resolver::query::canonical_name);
    mResolver.async_resolve(query, boost::bind(&SbsClient::handleResolve, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::iterator));
}

void SbsClient::handleResolve(const boost::system::error_code& cr_ec,
                              boost::asio::ip::tcp::resolver::iterator it) noexcept
{
    if(!cr_ec)
    {
        boost::asio::async_connect(mSocket, it,
                                   boost::bind(&SbsClient::handleConnect, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::iterator));
    }
    else
    {
        Logger::error("(SbsClient) resolve host: ", cr_ec.message());
        if(mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void SbsClient::handleConnect(const boost::system::error_code& cr_ec,
                              boost::asio::ip::tcp::resolver::iterator it) noexcept
{
    if(!cr_ec)
    {
        mSocket.set_option(boost::asio::socket_base::keep_alive(true));
        Logger::info("(SbsClient) connected to: ", mHost, ":", mPort);
        read();
    }
    else
    {
        Logger::error("(SbsClient) connect: ", cr_ec.message());
        if(mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

}  // namespace client
}  // namespace network
