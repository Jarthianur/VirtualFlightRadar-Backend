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

#include "SensorClient.h"

#include <utility>

#include "../util/Logger.hpp"

namespace client
{
SensorClient::SensorClient(const Endpoint& endpoint, std::shared_ptr<Connector> connector)
    : Client(endpoint, "(SensorClient)", connector)
{}

SensorClient::~SensorClient() noexcept
{}

void SensorClient::read()
{
    m_connector->resetTimer(WC_RCV_TIMEOUT);
    Client::read();
}

void SensorClient::checkDeadline(bool error)
{
    if(!error)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_connector->timerExpired())
        {
            logger.debug(m_component, " timed out, reconnect ...");
            reconnect();
        }
        else
        {
            m_connector->onTimeout(
                std::bind(&SensorClient::checkDeadline, this, std::placeholders::_1));
        }
    }
}

void SensorClient::handleConnect(bool error)
{
    if(!error)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        logger.info(m_component, " connected to ", m_endpoint.host, ":", m_endpoint.port);
        m_connector->onTimeout(std::bind(&SensorClient::checkDeadline, this, std::placeholders::_1),
                               WC_RCV_TIMEOUT);
        read();
    }
    else
    {
        logger.warn(m_component, " failed to connect to ", m_endpoint.host, ":", m_endpoint.port);
        reconnect();
    }
}

}  // namespace client
