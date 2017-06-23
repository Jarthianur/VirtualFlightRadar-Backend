/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstddef>
#include "../../config/Configuration.h"
#include "../../feed/Feed.h"
#include "../../util/Logger.h"

using namespace util;

namespace tcp
{
namespace client
{

GpsdClient::GpsdClient(const std::string& cr_host,
        const std::string& cr_port, feed::Feed& r_feed)
        : Client(cr_host, cr_port, "(GpsdClient)", r_feed)
{
    connect();
}

GpsdClient::~GpsdClient() noexcept
{
}

void GpsdClient::connect() noexcept
{
    boost::asio::ip::tcp::resolver::query query(mHost, mPort,
            boost::asio::ip::tcp::resolver::query::canonical_name);
    mResolver.async_resolve(query,
            boost::bind(&GpsdClient::handleResolve, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::iterator));
}

void GpsdClient::handleResolve(const boost::system::error_code& cr_ec,
        boost::asio::ip::tcp::resolver::iterator it)
        noexcept
        {
    if (!cr_ec)
    {
        boost::asio::async_connect(mSocket, it,
                boost::bind(&GpsdClient::handleConnect, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::iterator));
    } else
    {
        Logger::error("(GpsdClient) resolve host: ", cr_ec.message());
        if (mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void GpsdClient::handleConnect(const boost::system::error_code& cr_ec,
        boost::asio::ip::tcp::resolver::iterator it)
        noexcept
        {
    if (!cr_ec)
    {
        mSocket.set_option(boost::asio::socket_base::keep_alive(true)); // necessary?
        boost::asio::async_write(mSocket,
                boost::asio::buffer("?WATCH={\"enable\":true,\"nmea\":true}\r\n"),
                boost::bind(&GpsdClient::handleWatch, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    } else
    {
        Logger::error("(GpsdClient) connect: ", cr_ec.message());
        if (mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void GpsdClient::stop() noexcept
{
    if (mSocket.is_open())
    {
        boost::asio::async_write(mSocket,
                boost::asio::buffer("?WATCH={\"enable\":false}\r\n"),
                [this](const boost::system::error_code& ec, std::size_t s)
                {
                    if (!ec)
                    {
                        Logger::info("(GpsdClient) stopped watch");
                    }
                    else
                    {
                        Logger::error("(GpsdClient) send un-watch request: ", ec.message());
                    }
                });
    }
    Client::stop();
}

void GpsdClient::handleWatch(const boost::system::error_code& cr_ec, std::size_t s)
noexcept
{
    if (!cr_ec)
    {
        Logger::info("(GpsdClient) connected to: ", mHost + ":" + mPort);
        read();
    } else
    {
        Logger::error("(GpsdClient) send watch request: ", cr_ec.message());
    }
}

}  // namespace client
}  // namespace tcp
