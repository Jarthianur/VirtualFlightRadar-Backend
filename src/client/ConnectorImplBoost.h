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

#pragma once

#include <functional>
#include <istream>
#include <string>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "../Defines.h"

namespace client
{
struct Endpoint;

using Callback     = std::function<void(bool)>;
using ReadCallback = std::function<void(bool, const std::string&)>;

class ConnectorImplBoost
{
public:
    NOT_COPYABLE(ConnectorImplBoost)

    ConnectorImplBoost();
    ~ConnectorImplBoost() noexcept;

    void run();
    void stop();
    void close();
    void onConnect(const Endpoint& endpoint, const Callback& callback);
    void onRead(const ReadCallback& crCallback);
    void onWrite(const std::string& msg, const Callback& callback);
    void onTimeout(const Callback& callback, std::uint32_t timeout = 0);
    void resetTimer(std::uint32_t timeout);
    bool timerExpired();

private:
    void handleResolve(const boost::system::error_code& error,
                       boost::asio::ip::tcp::resolver::iterator resolverIt,
                       const Callback& callback) noexcept;

    void handleConnect(const boost::system::error_code& error,
                       boost::asio::ip::tcp::resolver::iterator resolverIt,
                       const Callback& callback) noexcept;

    void handleTimeout(const boost::system::error_code& error,
                       const Callback& callback) noexcept;

    void handleRead(const boost::system::error_code& error, std::size_t bytes,
                    const ReadCallback& callback) noexcept;

    void handleWrite(const boost::system::error_code& error, std::size_t bytes,
                     const Callback& callback) noexcept;

    boost::asio::io_service m_ioService;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::ip::tcp::resolver m_resolver;
    boost::asio::deadline_timer m_timer;
    boost::asio::streambuf m_buffer;
    std::string m_response;
    std::istream m_istream;
};

}  // namespace client
