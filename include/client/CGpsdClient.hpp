/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "IClient.hpp"

namespace vfrb::client
{
/**
 * @brief Client for GPSD servers
 */
class CGpsdClient : public IClient
{
    /**
     * @brief Send unwatch-request and stop this client.
     */
    void stop() override REQUIRES(m_mutex);

    /**
     * @brief Implement Client::handleConnect
     * @threadsafe
     */
    void handleConnect(net::EErrc err_) override REQUIRES(!m_mutex);

    /**
     * @brief Handler for watch-request sending
     * @param error The error indicator
     * @threadsafe
     */
    void handleWatch(net::EErrc err_) REQUIRES(!m_mutex);

    str logPrefix() const override;

public:
    /**
     * @param endpoint  The remote endpoint
     * @param connector The Connector interface
     */
    CGpsdClient(net::SEndpoint const& ep_, SPtr<net::IConnector> con_);
};
}  // namespace vfrb::client
