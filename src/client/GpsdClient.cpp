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

#include "GpsdClient.h"

#include <chrono>
#include <thread>
#include <utility>

#include "../Logger.hpp"

namespace client
{
GpsdClient::GpsdClient(const Endpoint& endpoint, std::shared_ptr<Connector> connector)
    : Client(endpoint, "(GpsdClient)", connector)
{}

GpsdClient::~GpsdClient() noexcept
{}

void GpsdClient::handleConnect(bool error)
{
    if(!error)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connector->onWrite("?WATCH={\"enable\":true,\"nmea\":true}\r\n",
                             std::bind(&GpsdClient::handleWatch, this, std::placeholders::_1));
    }
    else
    {
        logger.warn(m_component, " failed to connect to ", m_endpoint.host, ":", m_endpoint.port);
        reconnect();
    }
}

void GpsdClient::stop()
{
    if(m_running)
    {
        m_connector->onWrite("?WATCH={\"enable\":false}\r\n",
                             [this](bool) { logger.info(m_component, " stopped watch"); });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    Client::stop();
}

void GpsdClient::handleWatch(bool error)
{
    if(!error)
    {
        logger.info(m_component, " connected to ", m_endpoint.host, ":", m_endpoint.port);
        std::lock_guard<std::mutex> lock(m_mutex);
        read();
    }
    else
    {
        logger.error(m_component, " send watch request failed");
    }
}

}  // namespace client
