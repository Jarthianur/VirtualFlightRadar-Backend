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

#include "client/IClient.hpp"

#include <algorithm>
#include <chrono>
#include <condition_variable>

#include "boost/functional/hash.hpp"
#include "client/net/IConnector.hpp"
#include "feed/IFeed.hpp"

#include "CLogger.hpp"

using vfrb::client::net::SEndpoint;
using vfrb::client::net::IConnector;
using vfrb::concurrent::ImmutableLock;
using vfrb::concurrent::MutableLock;

static auto const& logger = vfrb::CLogger::Instance();

namespace vfrb::client
{
namespace
{
constexpr auto STOP_COND_WAIT_TIME = 100;
}  // namespace

IClient::IClient(SEndpoint const& ep_, Shared<IConnector> con_) : m_connector(con_), m_endpoint(ep_) {}

auto
IClient::Equals(IClient const& other_) const -> bool {
    return this->m_endpoint == other_.m_endpoint;
}

auto
IClient::Hash() const -> usize {
    usize seed = 0;
    boost::hash_combine(seed, boost::hash_value(m_endpoint.Host));
    boost::hash_combine(seed, boost::hash_value(m_endpoint.Port));
    return seed;
}

void
IClient::Subscribe(Shared<feed::IFeed> feed_) {
    ImmutableLock lk(m_mutex);
    m_feeds.push_back(feed_);
    std::sort(m_feeds.begin(), m_feeds.end(),
              [](Shared<feed::IFeed> const& f1_, Shared<feed::IFeed> const& f2_) {
                  return f1_->Priority() > f2_->Priority();
              });
}

void
IClient::Run() NO_THREAD_SAFETY_ANALYSIS {
    MutableLock lk(m_mutex);
    if (m_state == EState::NONE) {
        connect();
        lk.unlock();
        m_connector->Run();
        m_state = EState::NONE;
    }
}

void
IClient::connect() {
    m_state = EState::CONNECTING;
    m_connector->OnConnect(m_endpoint, [this](Result<void>&& res_) { handleConnect(res_); });
}

void
IClient::reconnect() {
    if (m_state != EState::STOPPING) {
        m_state = EState::CONNECTING;
        logger.Info(logPrefix(), "schedule reconnect to ", m_endpoint.Host, ":", m_endpoint.Port);
        m_connector->Close();
        timedConnect();
    }
}

void
IClient::timedConnect() {
    m_connector->OnTimeout([this](Result<void>&& res_) { handleTimedConnect(res_); }, m_backoff.Next());
}

void
IClient::stop() {
    if (m_state == EState::RUNNING || m_state == EState::CONNECTING) {
        m_state = EState::STOPPING;
        logger.Info(logPrefix(), "disconnect from ", m_endpoint.Host, ":", m_endpoint.Port);
        m_connector->Stop();
    }
}

void
IClient::ScheduleStop() NO_THREAD_SAFETY_ANALYSIS {
    std::condition_variable_any cond_ready;
    MutableLock                 lk(m_mutex);
    cond_ready.wait_for(lk, std::chrono::milliseconds(STOP_COND_WAIT_TIME),
                        [this]() NO_THREAD_SAFETY_ANALYSIS { return m_state != EState::NONE; });
    stop();
}

void
IClient::read() {
    m_connector->OnRead([this](Result<String>&& res_) { handleRead(std::move(res_)); });
}

void
IClient::handleTimedConnect(Result<void> res_) {
    if (res_) {
        ImmutableLock lk(m_mutex);
        logger.Info(logPrefix(), "try connect to ", m_endpoint.Host, ":", m_endpoint.Port);
        connect();
    } else {
        logger.Error(logPrefix(), "failed to connect after timeout");
        ScheduleStop();
    }
}

void
IClient::handleRead(Result<String>&& res_) {
    ImmutableLock lk(m_mutex);
    if (m_state == EState::RUNNING) {
        if (res_) {
            for (auto& it : m_feeds) {
                if (!it->Consume(std::move(res_.Value))) {
                    stop();
                }
            }
            read();
        } else {
            logger.Error(logPrefix(), "failed to read message");
            reconnect();
        }
    }
}
}  // namespace vfrb::client
