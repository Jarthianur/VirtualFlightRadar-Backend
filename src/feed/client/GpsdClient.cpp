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

#include "GpsdClient.h"

#include <boost/bind.hpp>

#include "../../Logger.hpp"

namespace feed
{
namespace client
{
GpsdClient::GpsdClient(const std::string& crHost, const std::string& crPort,
                       feed::Feed& rFeed)
    : Client(crHost, crPort, "(GpsdClient)", rFeed)
{
    connect();
}

GpsdClient::~GpsdClient() noexcept
{}

void GpsdClient::connect()
{
    boost::asio::ip::tcp::resolver::query query(
        mHost, mPort, boost::asio::ip::tcp::resolver::query::canonical_name);
    mResolver.async_resolve(query, boost::bind(&GpsdClient::handleResolve, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::iterator));
}

void GpsdClient::handleResolve(
    const boost::system::error_code& crError,
    boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept
{
    if(!crError)
    {
        boost::asio::async_connect(mSocket, vResolverIt,
                                   boost::bind(&GpsdClient::handleConnect, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::iterator));
    }
    else
    {
        Logger::error("(GpsdClient) resolve host: ", crError.message());
        if(mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void GpsdClient::handleConnect(const boost::system::error_code& crError,
                               boost::asio::ip::tcp::resolver::iterator) noexcept
{
    if(!crError)
    {
        mSocket.set_option(boost::asio::socket_base::keep_alive(true));
        boost::asio::async_write(
            mSocket, boost::asio::buffer("?WATCH={\"enable\":true,\"nmea\":true}\r\n"),
            boost::bind(&GpsdClient::handleWatch, this, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        Logger::error("(GpsdClient) connect: ", crError.message());
        if(mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void GpsdClient::stop()
{
    if(mSocket.is_open())
    {
        boost::asio::async_write(
            mSocket, boost::asio::buffer("?WATCH={\"enable\":false}\r\n"),
            [this](const boost::system::error_code& crError, std::size_t) {
                if(!crError)
                {
                    Logger::info("(GpsdClient) stopped watch");
                }
                else
                {
                    Logger::error("(GpsdClient) send un-watch request: ",
                                  crError.message());
                }
            });
    }
    Client::stop();
}

void GpsdClient::handleWatch(const boost::system::error_code& crError,
                             std::size_t) noexcept
{
    if(!crError)
    {
        Logger::info("(GpsdClient) connected to: ", mHost, ":", mPort);
        read();
    }
    else
    {
        Logger::error("(GpsdClient) send watch request: ", crError.message());
    }
}

}  // namespace client
}  // namespace network
