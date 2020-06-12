/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "client/CSensorClient.hpp"

#include "client/net/IConnector.hpp"
#include "client/net/SEndpoint.hpp"
#include "concurrent/Mutex.hpp"

#include "CLogger.hpp"

using vfrb::client::net::SEndpoint;
using vfrb::client::net::IConnector;
using vfrb::client::net::EErrc;
using vfrb::concurrent::LockGuard;

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
            m_connector->OnTimeout([this](EErrc err_) { checkDeadline(err_); }, 0);
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
            m_connector->OnTimeout([this](EErrc err_) { checkDeadline(err_); }, RECEIVE_TIMEOUT);
            read();
        } else {
            logger.Warn(LOG_PREFIX, "failed to connect to ", m_endpoint.Host, ":", m_endpoint.Port);
            reconnect();
        }
    }
}

auto CSensorClient::logPrefix() const -> str {
    return LOG_PREFIX;
}
}  // namespace vfrb::client
