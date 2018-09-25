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

#include <cstdint>
#include <functional>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "SocketImplBoost.h"
#include "TcpInterface.hpp"

#include "../Defines.h"

namespace server
{
class TcpInterfaceImplBoost : public TcpInterface<SocketImplBoost>
{
public:
    NOT_COPYABLE(TcpInterfaceImplBoost)

    explicit TcpInterfaceImplBoost(std::uint16_t port);
    ~TcpInterfaceImplBoost() noexcept;

    void run(std::unique_lock<std::mutex>& lock) override;
    void stop() override;
    void onAccept(const std::function<void(bool)>& callback) override;
    void close() override;
    std::unique_ptr<Connection<SocketImplBoost>> startConnection() override;
    std::string get_currentAddress() const override;

private:
    void handleAccept(const boost::system::error_code& error,
                      const std::function<void(bool)>& callback) noexcept;

    boost::asio::io_service m_ioService;
    boost::asio::ip::tcp::acceptor m_acceptor;
    SocketImplBoost m_socket;
};

}  // namespace server
