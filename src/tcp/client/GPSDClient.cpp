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

#include "GPSDClient.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstddef>
#include "../../config/Configuration.h"
#include "../../vfrb/Feed.h"
#include "../../util/Logger.h"

GPSDClient::GPSDClient(boost::asio::signal_set& r_sigset,
                       const std::string& cr_host, const std::string& cr_port,
                       Feed& r_feed)
        : Client(r_sigset, cr_host, cr_port, "(GPSDClient)", r_feed)
{
    connect();
}

GPSDClient::~GPSDClient() noexcept
{
}

void GPSDClient::connect() noexcept
{
    boost::asio::ip::tcp::resolver::query query(mHost, mPort,
            boost::asio::ip::tcp::resolver::query::canonical_name);
    mResolver.async_resolve(query,
            boost::bind(&GPSDClient::handleResolve, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::iterator));
}

void GPSDClient::process() noexcept
{
    if (mrFeed.process(mResponse) > 0 && Configuration::global_gnd_mode)
    {
        Logger::info("(GPSDClient) received good position -> stop");
        stop();
    }
}

void GPSDClient::handleResolve(const boost::system::error_code& cr_ec,
                               boost::asio::ip::tcp::resolver::iterator it)
                                       noexcept
                                       {
    if (!cr_ec)
    {
        boost::asio::async_connect(mSocket, it,
                boost::bind(&GPSDClient::handleConnect, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::iterator));
    } else
    {
        Logger::error("(GPSDClient) resolve host: ", cr_ec.message());
        if (mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void GPSDClient::handleConnect(const boost::system::error_code& cr_ec,
                               boost::asio::ip::tcp::resolver::iterator it)
                                       noexcept
                                       {
    if (!cr_ec)
    {
        mSocket.set_option(boost::asio::socket_base::keep_alive(true)); // necessary?
        boost::asio::async_write(mSocket,
                boost::asio::buffer(
                        "?WATCH={\"enable\":true,\"nmea\":true}\r\n"),
                boost::bind(&GPSDClient::handleWatch, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    } else
    {
        Logger::error("(GPSDClient) connect: ", cr_ec.message());
        if (mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void GPSDClient::stop() noexcept
{
    if (mSocket.is_open())
    {
        boost::asio::async_write(mSocket,
                boost::asio::buffer("?WATCH={\"enable\":false}\r\n"),
                [this](const boost::system::error_code& ec, std::size_t s)
                {
                    if (!ec)
                    {
                        Logger::info("(GPSDClient) stopped watch");
                    }
                    else
                    {
                        Logger::error("(GPSDClient) send un-watch request: ", ec.message());
                    }
                });
    }
    Client::stop();
}

void GPSDClient::handleWatch(const boost::system::error_code& cr_ec,
                             std::size_t s) noexcept
                             {
    if (!cr_ec)
    {
        Logger::info("(GPSDClient) connected to: ", mHost + ":" + mPort);
        read();
    } else
    {
        Logger::error("(GPSDClient) send watch request: ", cr_ec.message());
    }
}
