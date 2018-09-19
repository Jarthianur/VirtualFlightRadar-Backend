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
    explicit GpsdClient(const Endpoint& endpoint);

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
    void handleConnect(bool error) noexcept override;

    /**
     * @fn handleWatch
     * @brief Handler for watch-request sending
     * @param crError The error code
     * @param vBytes  The sent bytes
     */
    void handleWatch(bool error) noexcept;
};

template<typename ConnectorT>
GpsdClient<ConnectorT>::GpsdClient(const Endpoint& endpoint)
    : Client<ConnectorT>(endpoint, "(GpsdClient)")
{}

template<typename ConnectorT>
GpsdClient<ConnectorT>::~GpsdClient() noexcept
{}

template<typename ConnectorT>
void GpsdClient<ConnectorT>::handleConnect(bool error) noexcept
{
    if(!error)
    {
        std::lock_guard<std::mutex> lock(this->m_mutex);
        this->m_connector.onWrite("?WATCH={\"enable\":true,\"nmea\":true}\r\n",
                                  std::bind(&GpsdClient::handleWatch, this, std::placeholders::_1));
    }
    else
    {
        logger.warn(this->m_component, " failed to connect to ", this->m_endpoint.host, ":",
                    this->m_endpoint.port);
        this->reconnect();
    }
}

template<typename ConnectorT>
void GpsdClient<ConnectorT>::stop()
{
    if(this->m_running)
    {
        this->m_connector.onWrite("?WATCH={\"enable\":false}\r\n", [this](bool) {
            logger.info(this->m_component, " stopped watch");
        });
    }
    Client<ConnectorT>::stop();
}

template<typename ConnectorT>
void GpsdClient<ConnectorT>::handleWatch(bool error) noexcept
{
    if(!error)
    {
        logger.info(this->m_component, " connected to ", this->m_endpoint.host, ":",
                    this->m_endpoint.port);
        std::lock_guard<std::mutex> lock(this->m_mutex);
        this->read();
    }
    else
    {
        logger.error(this->m_component, " send watch request failed");
    }
}

}  // namespace client
