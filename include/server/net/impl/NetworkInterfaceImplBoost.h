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

#include "server/net/NetworkInterface.hpp"
#include "server/net/impl/SocketImplBoost.h"
#include "util/defines.h"

namespace server
{
namespace net
{
/**
 * @brief Implement NetworkInterface using boost.
 */
class NetworkInterfaceImplBoost : public NetworkInterface<SocketImplBoost>
{
public:
    NOT_COPYABLE(NetworkInterfaceImplBoost)

    /**
     * @brief Constructor
     * @param port The port number
     */
    explicit NetworkInterfaceImplBoost(std::uint16_t port);

    ~NetworkInterfaceImplBoost() noexcept;

    /**
     * @brief Run the event handler queue.
     * @note Blocks until all handlers have returned.
     * @param lock The lock to release before entering blocking section
     */
    void run(std::unique_lock<std::mutex>& lock) override;

    /**
     * @brief Stop the event handler queue.
     */
    void stop() override;

    /**
     * @brief Schedule an accept call.
     * @param callback The callback to invoke when done
     */
    void onAccept(const std::function<void(bool)>& callback) override;

    /**
     * @brief Close current connection.
     */
    void close() override;

    /**
     * @brief Start and get the Connection on current socket.
     * @return the Connection
     * @throw SocketException if the current socket is not open
     */
    std::unique_ptr<Connection<SocketImplBoost>> startConnection() override;

    /**
     * @brief Get the current connected address.
     * @return the address
     * @throw SocketException if the current socket is not open
     */
    std::string get_currentAddress() const override;

private:
    /**
     * @brief Handler for accept calls
     * @param error    The error code
     * @param callback The callback to invoke
     */
    void handleAccept(const boost::system::error_code& error,
                      const std::function<void(bool)>& callback);

    /// Internal IO-service
    boost::asio::io_service m_ioService;

    /// Acceptor
    boost::asio::ip::tcp::acceptor m_acceptor;

    /// Current socket
    SocketImplBoost m_socket;
};
}  // namespace net
}  // namespace server
