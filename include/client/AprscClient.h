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
#include "types.h"

namespace vfrb::client
{
/**
 * @brief Client for APRSC servers
 */
class CAprscClient : public IClient
{
    NOT_COPYABLE(CAprscClient)

    inline static constexpr auto KEEPALIVE_INTERVAL = 600;

    Str const m_login;  ///< Login string

    /**
     * @brief Schedule sending of a keep-alive beacon.
     */
    void sendKeepAlive() REQUIRES(m_mutex);

    /**
     * @brief Implement Client::handleConnect
     * @threadsafe
     */
    void handleConnect(net::EErrc err_) override REQUIRES(!m_mutex);

    /**
     * @brief Handler for sending of the login string.
     * @param error The error indicator
     * @threadsafe
     */
    void handleLogin(net::EErrc err_) REQUIRES(!m_mutex);

    /**
     * @brief Handler for sending a keep-alive beacon.
     * @param error The error indicator
     * @threadsafe
     */
    void handleSendKeepAlive(net::EErrc err_) REQUIRES(!m_mutex);

    char const* logPrefix() const override;

public:
    /**
     * @param endpoint  The remote endpoint
     * @param login     The login string
     * @param connector The Connector interface
     */
    CAprscClient(net::SEndpoint const& ep_, Str const& login_, SPtr<net::IConnector> con_);
    ~CAprscClient() noexcept override = default;

    bool  Equals(IClient const& other_) const override;
    usize Hash() const override;
};
}  // namespace vfrb::client
