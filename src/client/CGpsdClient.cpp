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

#include "client/CGpsdClient.hpp"

#include <chrono>
#include <condition_variable>
#include <thread>
#include <utility>

#include "client/net/IConnector.hpp"
#include "client/net/SEndpoint.hpp"
#include "concurrent/Mutex.hpp"

#include "CLogger.hpp"

using vfrb::client::net::SEndpoint;
using vfrb::client::net::IConnector;
using vfrb::concurrent::LockGuard;
using vfrb::concurrent::UniqueLock;
using vfrb::concurrent::Mutex;

namespace vfrb::client
{
namespace
{
constexpr auto STOP_COND_WAIT_TIME = 500;
}  // namespace

CTCONST            LOG_PREFIX = "(GpsdClient) ";
static auto const& logger     = CLogger::Instance();

CGpsdClient::CGpsdClient(SEndpoint const& ep_, SPtr<IConnector> con_) : IClient(ep_, con_) {}

void
CGpsdClient::handleConnect(Result<void> res_) {
    LockGuard lk(m_mutex);
    if (m_state == EState::CONNECTING) {
        if (res_) {
            m_connector->OnWrite("?WATCH={\"enable\":true,\"nmea\":true}\r\n",
                                 [this](Result<void> res_) { handleWatch(res_); });
        } else {
            logger.Warn(LOG_PREFIX, "failed to connect to ", m_endpoint.Host, ":", m_endpoint.Port);
            reconnect();
        }
    }
}

void
CGpsdClient::stop() {
    if (m_state == EState::RUNNING) {
        Mutex                       sync;
        UniqueLock                  lk(sync);
        std::condition_variable_any cv;
        bool                        sent = false;
        m_connector->OnWrite("?WATCH={\"enable\":false}\r\n", [&sent, &cv]([[maybe_unused]] Result<void>) {
            logger.Info(LOG_PREFIX, "stopped watch");
            sent = true;
            cv.notify_one();
        });
        cv.wait_for(lk, std::chrono::milliseconds(STOP_COND_WAIT_TIME), [&] { return sent; });
    }
    IClient::stop();
}

void
CGpsdClient::handleWatch(Result<void> res_) {
    LockGuard lk(m_mutex);
    if (m_state == EState::CONNECTING) {
        if (res_) {
            m_state = EState::RUNNING;
            m_backoff.Reset();
            logger.Info(LOG_PREFIX, "connected to ", m_endpoint.Host, ":", m_endpoint.Port);
            read();
        } else {
            logger.Error(LOG_PREFIX, "send watch request failed");
            reconnect();
        }
    }
}

auto
CGpsdClient::logPrefix() const -> str {
    return LOG_PREFIX;
}
}  // namespace vfrb::client
