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
 * @brief Client for sensor servers
 */
class SensorClient : public Client
{
    //< begin methods >//
    /**
     * @brief Override Client::read, use timeout
     */
    void read() override;

    /**
     * @brief Check read timeout deadline reached.
     * @threadsafe
     */
    void checkDeadline(bool error);

    /**
     * @brief Implement Client::handleConnect
     * @threadsafe
     */
    void handleConnect(bool error) override;
    //< end methods >//

    //< begin constants >//
    static constexpr auto LOG_PREFIX = "(SensorClient) ";
    //< end constants >//

public:
    NOT_COPYABLE(SensorClient)

    /**
     * @param endpoint  The remote endpoint
     * @param connector The Connector interface
     */
    SensorClient(const net::Endpoint& endpoint, std::shared_ptr<net::Connector> connector);
    ~SensorClient() noexcept override = default;
};

}  // namespace client
