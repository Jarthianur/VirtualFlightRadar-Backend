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

#include "client/CSbsClient.hpp"

#include "client/net/IConnector.hpp"
#include "client/net/SEndpoint.hpp"
#include "concurrent/Mutex.hpp"

#include "CLogger.hpp"

using vfrb::client::net::SEndpoint;
using vfrb::client::net::IConnector;
using vfrb::concurrent::ImmutableLock;

namespace vfrb::client
{
CTCONST            LOG_PREFIX = "(SbsClient) ";
static auto const& logger     = CLogger::Instance();

CSbsClient::CSbsClient(SEndpoint const& ep_, Shared<IConnector> con_) : IClient(ep_, con_) {}

void
CSbsClient::handleConnect(Result<void> res_) {
    ImmutableLock lk(m_mutex);
    if (m_state == EState::CONNECTING) {
        if (res_) {
            m_state = EState::RUNNING;
            m_backoff.Reset();
            logger.Info(LOG_PREFIX, "connected to ", m_endpoint.Host, ":", m_endpoint.Port);
            read();
        } else {
            logger.Warn(LOG_PREFIX, "failed to connect to ", m_endpoint.Host, ":", m_endpoint.Port);
            reconnect();
        }
    }
}

auto
CSbsClient::logPrefix() const -> str {
    return LOG_PREFIX;
}
}  // namespace vfrb::client
