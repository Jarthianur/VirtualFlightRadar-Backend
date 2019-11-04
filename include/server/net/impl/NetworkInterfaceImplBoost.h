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

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "server/net/NetworkInterface.hpp"
#include "server/net/impl/SocketImplBoost.h"
#include "util/class_utils.h"

#include "types.h"

namespace vfrb::server::net
{
/**
 * @brief Implement NetworkInterface using boost.
 */
class NetworkInterfaceImplBoost : public NetworkInterface<SocketImplBoost>
{
    NOT_COPYABLE(NetworkInterfaceImplBoost)

    boost::asio::io_service        m_ioService;  ///< Internal IO-service
    boost::asio::ip::tcp::acceptor m_acceptor;   ///< Acceptor
    SocketImplBoost                m_socket;     ///< Current socket

    /**
     * @brief Handler for accept calls
     * @param error    The error code
     * @param callback The callback to invoke
     */
    void handleAccept(boost::system::error_code const& error, Callback const& callback);

public:
    explicit NetworkInterfaceImplBoost(u16 port);  ///< @param port The port number
    ~NetworkInterfaceImplBoost() noexcept override;

    /**
     * @brief Run the event handler queue.
     * @note Blocks until all handlers have returned.
     * @param lock The lock to release before entering blocking section
     */
    void run(std::unique_lock<std::mutex>& lk) override;

    /**
     * @brief Stop the event handler queue.
     */
    void stop() override;

    /**
     * @brief Schedule an accept call.
     * @param callback The callback to invoke when done
     */
    void onAccept(Callback const& callback) override;

    /**
     * @brief Close current connection.
     */
    void close() override;

    /**
     * @brief Start and get the Connection on current socket.
     * @return the Connection
     * @throw SocketException if the current socket is not open
     */
    Connection<SocketImplBoost> startConnection() override;

    /**
     * @brief Get the current connected address.
     * @return the address
     * @throw SocketException if the current socket is not open
     */
    str stagedAddress() const override;
};
}  // namespace vfrb::server::net
