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

#include "../../util/Logger.h"

GPSDClient::GPSDClient(boost::asio::signal_set& sigset, const std::string& host,
                       const std::string& port, Feed& feed)
        : Client(sigset, host, port, "(GPSDClient)", feed)
{
    connect();
}

GPSDClient::~GPSDClient() noexcept
{
}

void GPSDClient::connect() noexcept
{
    boost::asio::ip::tcp::resolver::query query(
            mHost, mPort, boost::asio::ip::tcp::resolver::query::canonical_name);
    mResolver.async_resolve(
            query,
            boost::bind(&GPSDClient::handleResolve, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::iterator));
}

void GPSDClient::handleResolve(const boost::system::error_code& ec,
                               boost::asio::ip::tcp::resolver::iterator it) noexcept
{
    if (!ec)
    {
        boost::asio::async_connect(
                mSocket,
                it,
                boost::bind(&GPSDClient::handleConnect, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::iterator));
    }
    else
    {
        Logger::error("(GPSDClient) resolve host: ", ec.message());
        if (mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void GPSDClient::handleConnect(const boost::system::error_code& ec,
                               boost::asio::ip::tcp::resolver::iterator it) noexcept
{
    if (!ec)
    {
        mSocket.set_option(boost::asio::socket_base::keep_alive(true)); // necessary?
        boost::asio::async_write(
                mSocket,
                boost::asio::buffer("?WATCH={\"enable\":true,\"nmea\":true}\r\n"),
                boost::bind(&GPSDClient::handleWatch, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        Logger::error("(GPSDClient) connect: ", ec.message());
        if (mSocket.is_open())
        {
            mSocket.close();
        }
        timedConnect();
    }
}

void GPSDClient::stop() noexcept
{
    boost::asio::async_write(
            mSocket, boost::asio::buffer("?WATCH={\"enable\":false}\r\n"),
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
    Client::stop();
}

void GPSDClient::handleWatch(const boost::system::error_code& ec, std::size_t s) noexcept
{
    if (!ec)
    {
        Logger::info("(GPSDClient) connected to: ", mHost + ":" + mPort);
        read();
    }
    else
    {
        Logger::error("(GPSDClient) send watch request: ", ec.message());
    }
}
