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
 * @class SbsClient
 * @brief Connect to a SBS server.
 * @extends Client
 */

class SbsClient : public Client
{
public:
    NOT_COPYABLE(SbsClient)

    /**
     * @fn SbsClient
     * @brief Constructor
     * @param crHost  The hostname
     * @param crPort  The port
     * @param rFeed   The handler Feed reference
     */
    SbsClient(const Endpoint& endpoint, std::shared_ptr<Connector> connector);

    /**
     * @fn ~SbsClient
     * @brief Destructor
     */
    ~SbsClient() noexcept;

private:
    /**
     * @see Client#handleConnect
     */
    void handleConnect(bool error) override;
};

}  // namespace client
