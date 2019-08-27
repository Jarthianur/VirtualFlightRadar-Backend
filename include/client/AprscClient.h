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

#include "util/types.h"

#include "Client.h"

namespace vfrb::client
{
/**
 * @brief Client for APRSC servers
 */
class AprscClient : public Client
{
    NOT_COPYABLE(AprscClient)

    str const m_login;  ///< Login string

    /**
     * @brief Schedule sending of a keep-alive beacon.
     */
    void sendKeepAlive();

    /**
     * @brief Implement Client::handleConnect
     * @threadsafe
     */
    void handleConnect(bool error) override;

    /**
     * @brief Handler for sending of the login string.
     * @param error The error indicator
     * @threadsafe
     */
    void handleLogin(bool error);

    /**
     * @brief Handler for sending a keep-alive beacon.
     * @param error The error indicator
     * @threadsafe
     */
    void handleSendKeepAlive(bool error);

    char const* logPrefix() const override;

public:
    /**
     * @param endpoint  The remote endpoint
     * @param login     The login string
     * @param connector The Connector interface
     */
    AprscClient(net::Endpoint const& endpoint, str const& login, s_ptr<net::Connector> connector);
    ~AprscClient() noexcept override = default;

    bool  equals(Client const& other) const override;
    usize hash() const override;
};
}  // namespace vfrb::client
