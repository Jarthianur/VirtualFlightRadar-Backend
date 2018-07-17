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

#include <string>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "../../Defines.h"

namespace feed
{
namespace client
{
class ConnectorImplBoost
{
public:
    NOT_COPYABLE(ConnectorImplBoost)

    ConnectorImplBoost();
    ~ConnectorImplBoost() noexcept;

    void run();
    void stop();
    void close();
    void onRead(const std::function<void(bool, const std::string&) noexcept>& crCallback);
    void onTimeout(std::uint32_t vTimeout, const std::function<void(bool) noexcept>& crCallback);

private:
    void handleTimeout(const boost::system::error_code& crError,
                       const std::function<void(bool) noexcept>& crCallback) noexcept;
    void
    handleRead(const boost::system::error_code& crError, std::size_t vBytes,
               const std::function<void(bool, const std::string&) noexcept>& crCallback) noexcept;

    boost::asio::io_service mIoService;
    boost::asio::ip::tcp::socket mSocket;
    boost::asio::ip::tcp::resolver mResolver;
    boost::asio::deadline_timer mConnectTimer;
    boost::asio::streambuf mBuffer;
    std::string mResponse;
};

}  // namespace client
}  // namespace feed
