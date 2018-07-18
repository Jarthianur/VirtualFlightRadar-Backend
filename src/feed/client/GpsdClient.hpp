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

/// @namespace feed
namespace feed
{
/// @namespace client
namespace client
{
/**
 * @class GpsdClient
 * @brief Connect to a GPSD server.
 * @extends Client
 */
template<typename ConnectorT>
class GpsdClient : public Client<ConnectorT>
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
    explicit GpsdClient(const Endpoint& crEndpoint);

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
    void handleConnect(bool vError) noexcept override;

    /**
     * @fn handleWatch
     * @brief Handler for watch-request sending
     * @param crError The error code
     * @param vBytes  The sent bytes
     */
    void handleWatch(bool vError) noexcept;
};

template<typename ConnectorT>
GpsdClient<ConnectorT>::GpsdClient(const Endpoint& crEndpoint)
    : Client<ConnectorT>(crEndpoint, "(GpsdClient)")
{}

template<typename ConnectorT>
GpsdClient<ConnectorT>::~GpsdClient() noexcept
{}

template<typename ConnectorT>
void GpsdClient<ConnectorT>::handleConnect(bool vError) noexcept
{
    if(vError)
    {
        boost::lock_guard<boost::mutex> lock(this->mMutex);
        this->mConnector.onWrite("?WATCH={\"enable\":true,\"nmea\":true}\r\n",
                                 std::bind(&GpsdClient::handleWatch, this, std::placeholders::_1));
    }
    else
    {
        logger.warn(this->mComponent, " failed to connect to ", this->mEndpoint.host, ":",
                    this->mEndpoint.port);
        this->reconnect();
    }
}

template<typename ConnectorT>
void GpsdClient<ConnectorT>::stop()
{
    if(this->mRunning)
    {
        this->mConnector.onWrite("?WATCH={\"enable\":false}\r\n",
                                 [this](bool) { logger.info(this->mComponent, " stopped watch"); });
    }
    Client<ConnectorT>::stop();
}

template<typename ConnectorT>
void GpsdClient<ConnectorT>::handleWatch(bool vError) noexcept
{
    if(vError)
    {
        logger.info(this->mComponent, " connected to ", this->mEndpoint.host, ":",
                    this->mEndpoint.port);
        boost::lock_guard<boost::mutex> lock(this->mMutex);
        this->read();
    }
    else
    {
        logger.error(this->mComponent, " send watch request failed");
    }
}

}  // namespace client
}  // namespace feed
