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

#include <cstddef>
#include <string>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "../../Defines.h"

#include "Client.h"

/// @namespace feed
namespace feed
{
/// @namespace client
namespace client
{
/**
 * @class AprscClient
 * @brief Connect to an APRSC server.
 * @extends Client
 */
class AprscClient : public Client
{
public:
    NOT_COPYABLE(AprscClient)

    /**
     * @fn AprscClient
     * @brief Constructor
     * @param crHost  The hostname
     * @param crPort  The port
     * @param crLogin The login string to transmit
     * @param rFeed   The handler Feed reference
     */
    AprscClient(const Endpoint& crEndpoint, const std::string& crLogin);

    /**
     * @fn ~AprscClient
     * @brief Destructor
     */
    ~AprscClient() noexcept;

    bool equals(const Client& crOther) const override;

    std::size_t hash() const override;

private:
    /**
     * @see Client#connect
     */
    void connect() override;

    /**
     * @see Client#stop
     */
    void stop() override;

    /**
     * @fn sendKeepAlive
     * @brief Send a keep-alive beacon every 10 minutes.
     */
    void sendKeepAlive();

    /**
     * @see Client#handleResolve
     */
    void handleResolve(const boost::system::error_code& crError,
                       boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept override;

    /**
     * @see Client#handleConnect
     */
    void handleConnect(const boost::system::error_code& crError,
                       boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept override;

    /**
     * @fn handleLogin
     * @brief Handler login string sending.
     * @param crError The error code
     * @param vBytes  The sent bytes
     */
    void handleLogin(const boost::system::error_code& crError, std::size_t vBytes) noexcept;

    /**
     * @fn handleSendKeepAlive
     * @brief Handler for sendKeepAlive
     * @param crError The error code
     * @param vBytes  The sent bytes
     */
    void handleSendKeepAlive(const boost::system::error_code& crError, std::size_t vBytes) noexcept;

    /// @var mLoginStr
    /// Login string
    std::string mLoginStr;

    /// @var mTimeout
    /// Beacon timer
    boost::asio::deadline_timer mTimeout;
};

}  // namespace client
}  // namespace feed
