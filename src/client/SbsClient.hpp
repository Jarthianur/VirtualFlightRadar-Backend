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

#include "Client.hpp"

/// @namespace client
namespace client
{
/**
 * @class SbsClient
 * @brief Connect to a SBS server.
 * @extends Client
 */
template<typename ConnectorT>
class SbsClient : public Client<ConnectorT>
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
    explicit SbsClient(const Endpoint& crEndpoint);

    /**
     * @fn ~SbsClient
     * @brief Destructor
     */
    ~SbsClient() noexcept;

private:
    /**
     * @see Client#handleConnect
     */
    void handleConnect(bool vError) noexcept override;
};

template<typename ConnectorT>
SbsClient<ConnectorT>::SbsClient(const Endpoint& crEndpoint)
    : Client<ConnectorT>(crEndpoint, "(SbsClient)")
{}

template<typename ConnectorT>
SbsClient<ConnectorT>::~SbsClient() noexcept
{}

template<typename ConnectorT>
void SbsClient<ConnectorT>::handleConnect(bool vError) noexcept
{
    if(vError)
    {
        std::lock_guard<std::mutex> lock(this->mMutex);
        this->read();
    }
    else
    {
        logger.warn(this->mComponent, " failed to connect to ", this->mEndpoint.host, ":",
                    this->mEndpoint.port);
        this->reconnect();
    }
}

}  // namespace client