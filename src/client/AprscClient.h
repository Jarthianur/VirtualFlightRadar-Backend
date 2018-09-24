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

#include "Client.h"

#include <string>

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
    AprscClient(const Endpoint& endpoint, const std::string& login,
                std::shared_ptr<Connector> connector);

    /**
     * @fn ~AprscClient
     * @brief Destructor
     */
    ~AprscClient() noexcept;

    bool equals(const Client& other) const override;

    std::size_t hash() const override;

private:
    /**
     * @fn sendKeepAlive
     * @brief Send a keep-alive beacon every 10 minutes.
     */
    void sendKeepAlive();

    /**
     * @see Client#handleConnect
     */
    void handleConnect(bool error) override;

    /**
     * @fn handleLogin
     * @brief Handler login string sending.
     * @param crError The error code
     * @param vBytes  The sent bytes
     */
    void handleLogin(bool error);

    /**
     * @fn handleSendKeepAlive
     * @brief Handler for sendKeepAlive
     * @param crError The error code
     * @param vBytes  The sent bytes
     */
    void handleSendKeepAlive(bool error);

    /// @var mLoginStr
    /// Login string
    const std::string m_login;
};

}  // namespace client
