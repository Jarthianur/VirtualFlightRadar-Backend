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

namespace client
{
/**
 * @brief Client for GPSD servers
 */
class GpsdClient : public Client
{
    NOT_COPYABLE(GpsdClient)

    /**
     * @brief Send unwatch-request and stop this client.
     */
    void stop() override;

    /**
     * @brief Implement Client::handleConnect
     * @threadsafe
     */
    void handleConnect(bool error) override;

    /**
     * @brief Handler for watch-request sending
     * @param error The error indicator
     * @threadsafe
     */
    void handleWatch(bool error);

public:
    /**
     * @param endpoint  The remote endpoint
     * @param connector The Connector interface
     */
    GpsdClient(net::Endpoint const& endpoint, s_ptr<net::Connector> connector);
    ~GpsdClient() noexcept override = default;
};
}  // namespace client
