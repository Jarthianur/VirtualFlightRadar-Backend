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

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <utility>

#include <boost/functional/hash.hpp>

#include "client/IClient.hpp"
#include "feed/IFeed.hpp"

#include "CLogger.hpp"

static auto const& logger = vfrb::CLogger::Instance();

using namespace vfrb::client::net;
using namespace vfrb::concurrent;

namespace vfrb::client
{
IClient::IClient(SEndpoint const& ep_, SPtr<IConnector> con_) : m_connector(con_), m_endpoint(ep_) {}

bool IClient::Equals(IClient const& other_) const {
    return this->m_endpoint == other_.m_endpoint;
}

usize IClient::Hash() const {
    usize seed = 0;
    boost::hash_combine(seed, boost::hash_value(m_endpoint.Host));
    boost::hash_combine(seed, boost::hash_value(m_endpoint.Port));
    return seed;
}

void IClient::Subscribe(SPtr<feed::IFeed> feed_) {
    LockGuard lk(m_mutex);
    m_feeds.push_back(feed_);
    std::sort(m_feeds.begin(), m_feeds.end(), [](SPtr<feed::IFeed> const& f1_, SPtr<feed::IFeed> const& f2_) {
        return f1_->Priority() > f2_->Priority();
    });
}

void IClient::Run() NO_THREAD_SAFETY_ANALYSIS {
    UniqueLock lk(m_mutex);
    if (m_state == EState::NONE) {
        connect();
        lk.unlock();
        m_connector->Run();
        m_state = EState::NONE;
    }
}

void IClient::connect() {
    m_state = EState::CONNECTING;
    m_connector->OnConnect(m_endpoint, std::bind(&IClient::handleConnect, this, std::placeholders::_1));
}

void IClient::reconnect() {
    if (m_state != EState::STOPPING) {
        m_state = EState::CONNECTING;
        logger.Info(logPrefix(), "schedule reconnect to ", m_endpoint.Host, ":", m_endpoint.Port);
        m_connector->Close();
        timedConnect();
    }
}

void IClient::timedConnect() {
    m_connector->OnTimeout(std::bind(&IClient::handleTimedConnect, this, std::placeholders::_1),
                           m_backoff.Next());
}

void IClient::stop() {
    if (m_state == EState::RUNNING || m_state == EState::CONNECTING) {
        m_state = EState::STOPPING;
        logger.Info(logPrefix(), "disconnect from ", m_endpoint.Host, ":", m_endpoint.Port);
        m_connector->Stop();
    }
}

void IClient::ScheduleStop() NO_THREAD_SAFETY_ANALYSIS {
    std::condition_variable_any cond_ready;
    UniqueLock                  lk(m_mutex);
    cond_ready.wait_for(lk, std::chrono::milliseconds(100),
                        [this]() NO_THREAD_SAFETY_ANALYSIS { return m_state != EState::NONE; });
    stop();
}

void IClient::read() {
    m_connector->OnRead(std::bind(&IClient::handleRead, this, std::placeholders::_1, std::placeholders::_2));
}

void IClient::handleTimedConnect(EErrc err_) {
    if (err_ == EErrc::OK) {
        LockGuard lk(m_mutex);
        logger.Info(logPrefix(), "try connect to ", m_endpoint.Host, ":", m_endpoint.Port);
        connect();
    } else {
        logger.Error(logPrefix(), "failed to connect after timeout");
        ScheduleStop();
    }
}

void IClient::handleRead(EErrc err_, String const& str_) {
    LockGuard lk(m_mutex);
    if (m_state == EState::RUNNING) {
        if (err_ == EErrc::OK) {
            for (auto& it : m_feeds) {
                if (!it->Process(str_)) {
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
