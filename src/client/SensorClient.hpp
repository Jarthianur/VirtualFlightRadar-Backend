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

#ifdef WINDCLIENT_RECEIVE_TIMEOUT
#define WC_RCV_TIMEOUT WINDCLIENT_RECEIVE_TIMEOUT
#else
#define WC_RCV_TIMEOUT 5
#endif

/// @namespace client
namespace client
{
/**
 * @class SensorClient
 * @brief Connect to a sender of sensoric information.
 * @extends Client
 */
template<typename ConnectorT>
class SensorClient : public Client<ConnectorT>
{
public:
    NOT_COPYABLE(SensorClient)

    /**
     * @fn SensorClient
     * @brief Constructor
     * @param crHost The hostname
     * @param crPort The port
     * @param rFeed  The handler Feed reference
     */
    explicit SensorClient(const Endpoint& crEndpoint);

    /**
     * @fn ~SensorClient
     * @brief Destructor
     */
    ~SensorClient() noexcept;

private:
    void read() override;

    /**
     * @fn checkDeadline
     * @brief Check read timeout deadline reached.
     */
    void checkDeadline(bool vError) noexcept;

    /**
     * @see Client#handleConnect
     */
    void handleConnect(bool vError) noexcept override;
};

template<typename ConnectorT>
SensorClient<ConnectorT>::SensorClient(const Endpoint& crEndpoint)
    : Client<ConnectorT>(crEndpoint, "(SensorClient)")
{}

template<typename ConnectorT>
SensorClient<ConnectorT>::~SensorClient() noexcept
{}

template<typename ConnectorT>
void SensorClient<ConnectorT>::read()
{
    this->mConnector.resetTimer(WC_RCV_TIMEOUT);
    Client<ConnectorT>::read();
}

template<typename ConnectorT>
void SensorClient<ConnectorT>::checkDeadline(bool vError) noexcept
{
    if(vError)
    {
        std::lock_guard<std::mutex> lock(this->mMutex);
        if(this->mConnector.timerExpired())
        {
            logger.debug(this->mComponent, " timed out, reconnect ...");
            this->reconnect();
        }
        else
        {
            this->mConnector.onTimeout(
                std::bind(&SensorClient::checkDeadline, this, std::placeholders::_1));
        }
    }
}

template<typename ConnectorT>
void SensorClient<ConnectorT>::handleConnect(bool vError) noexcept
{
    if(vError)
    {
        std::lock_guard<std::mutex> lock(this->mMutex);
        logger.info(this->mComponent, " connected to ", this->mEndpoint.host, ":",
                    this->mEndpoint.port);
        this->mConnector.onTimeout(
            std::bind(&SensorClient::checkDeadline, this, std::placeholders::_1), WC_RCV_TIMEOUT);
        read();
    }
    else
    {
        logger.warn(this->mComponent, " failed to connect to ", this->mEndpoint.host, ":",
                    this->mEndpoint.port);
        this->reconnect();
    }
}

}  // namespace client