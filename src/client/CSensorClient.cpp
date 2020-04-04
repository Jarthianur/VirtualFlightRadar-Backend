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

#include "client/CSensorClient.hpp"

#include <utility>

#include "CLogger.hpp"

using namespace vfrb::client::net;
using namespace vfrb::concurrent;

namespace vfrb::client
{
CTCONST            LOG_PREFIX = "(SensorClient) ";
static auto const& logger     = CLogger::Instance();

CSensorClient::CSensorClient(SEndpoint const& ep_, SPtr<IConnector> con_) : IClient(ep_, con_) {}

void CSensorClient::read() {
    m_connector->ResetTimer(RECEIVE_TIMEOUT);
    IClient::read();
}

void CSensorClient::checkDeadline(EErrc err_) {
    LockGuard lk(m_mutex);
    if (m_state == EState::RUNNING) {
        if (err_ == EErrc::OK) {
            logger.Debug(LOG_PREFIX, "timed out, reconnect ...");
            reconnect();
        } else {
            m_connector->OnTimeout(std::bind(&CSensorClient::checkDeadline, this, std::placeholders::_1));
        }
    }
}

void CSensorClient::handleConnect(EErrc err_) {
    LockGuard lk(m_mutex);
    if (m_state == EState::CONNECTING) {
        if (err_ == EErrc::OK) {
            m_state = EState::RUNNING;
            m_backoff.Reset();
            logger.Info(LOG_PREFIX, "connected to ", m_endpoint.Host, ":", m_endpoint.Port);
            m_connector->OnTimeout(std::bind(&CSensorClient::checkDeadline, this, std::placeholders::_1),
                                   RECEIVE_TIMEOUT);
            read();
        } else {
            logger.Warn(LOG_PREFIX, "failed to connect to ", m_endpoint.Host, ":", m_endpoint.Port);
            reconnect();
        }
    }
}

str CSensorClient::logPrefix() const {
    return LOG_PREFIX;
}
}  // namespace vfrb::client
