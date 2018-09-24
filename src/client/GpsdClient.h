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

/// @namespace client
namespace client
{
/**
 * @class GpsdClient
 * @brief Connect to a GPSD server.
 * @extends Client
 */

class GpsdClient : public Client
{
public:
    NOT_COPYABLE(GpsdClient)

    /**
     * @fn GpsdClient
     * @brief Constructor
     * @param crHost The hostname
     * @param crPort The port
     * @param rFeed  The handler Feed reference
     */
    GpsdClient(const Endpoint& endpoint, std::shared_ptr<Connector> connector);

    /**
     * @fn ~GpsdClient
     * @brief Destructor
     */
    ~GpsdClient() noexcept;

private:
    /**
     * @fn stop
     * @brief Send unwatch-request before stop.
     * @see Client#stop
     */
    void stop() override;

    /**
     * @see Client#handleConnect
     */
    void handleConnect(bool error) override;

    /**
     * @fn handleWatch
     * @brief Handler for watch-request sending
     * @param crError The error code
     * @param vBytes  The sent bytes
     */
    void handleWatch(bool error);
};

}  // namespace client
