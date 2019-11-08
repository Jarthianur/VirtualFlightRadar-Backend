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

#include "client/SensorClient.h"

#include <utility>

#include "util/Logger.hpp"

using namespace vfrb::client::net;
using namespace vfrb::concurrent;

namespace vfrb::client
{
constexpr auto     LOG_PREFIX = "(SensorClient) ";
static auto const& logger     = CLogger::Instance();

SensorClient::SensorClient(Endpoint const& endpoint, SPtr<Connector> connector) : Client(endpoint, connector)
{}

void SensorClient::read()
{
    m_connector->resetTimer(RECEIVE_TIMEOUT);
    Client::read();
}

void SensorClient::checkDeadline(ErrorCode error)
{
    LockGuard lk(m_mutex);
    if (m_state == State::RUNNING)
    {
        if (error == ErrorCode::SUCCESS)
        {
            logger.debug(LOG_PREFIX, "timed out, reconnect ...");
            reconnect();
        }
        else
        {
            m_connector->onTimeout(std::bind(&SensorClient::checkDeadline, this, std::placeholders::_1));
        }
    }
}

void SensorClient::handleConnect(ErrorCode error)
{
    LockGuard lk(m_mutex);
    if (m_state == State::CONNECTING)
    {
        if (error == ErrorCode::SUCCESS)
        {
            m_state = State::RUNNING;
            m_backoff.reset();
            logger.info(LOG_PREFIX, "connected to ", m_endpoint.host, ":", m_endpoint.port);
            m_connector->onTimeout(std::bind(&SensorClient::checkDeadline, this, std::placeholders::_1),
                                   RECEIVE_TIMEOUT);
            read();
        }
        else
        {
            logger.warn(LOG_PREFIX, "failed to connect to ", m_endpoint.host, ":", m_endpoint.port);
            reconnect();
        }
    }
}

char const* SensorClient::logPrefix() const
{
    return LOG_PREFIX;
}
}  // namespace vfrb::client
