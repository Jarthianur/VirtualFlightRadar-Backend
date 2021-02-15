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

#include "client/CAprscClient.hpp"

#include <stdexcept>

#include <boost/functional/hash.hpp>

#include "client/net/IConnector.hpp"

#include "CLogger.hpp"

using vfrb::client::net::SEndpoint;
using vfrb::client::net::IConnector;
using vfrb::client::net::EErrc;
using vfrb::concurrent::LockGuard;

namespace vfrb::client
{
CTCONST            LOG_PREFIX = "(AprscClient) ";
static auto const& logger     = CLogger::Instance();

CAprscClient::CAprscClient(SEndpoint const& ep_, String const& login_, SPtr<IConnector> con_)
    : IClient(ep_, con_), m_login(login_ + "\r\n") {}

auto
CAprscClient::Equals(IClient const& other_) const -> bool {
    try {
        auto const& other = dynamic_cast<CAprscClient const&>(other_);
        return IClient::Equals(other_) && this->m_login == other.m_login;
    } catch ([[maybe_unused]] std::bad_cast const&) {
        return false;
    }
}

auto
CAprscClient::Hash() const -> usize {
    usize seed = IClient::Hash();
    boost::hash_combine(seed, boost::hash_value(m_login));
    return seed;
}

void
CAprscClient::handleConnect(EErrc err_) {
    LockGuard lk(m_mutex);
    if (m_state == EState::CONNECTING) {
        if (err_ == EErrc::OK) {
            m_connector->OnWrite(m_login, [this](EErrc err_) { handleLogin(err_); });
        } else {
            logger.Warn(LOG_PREFIX, "failed to connect to ", m_endpoint.Host, ":", m_endpoint.Port);
            reconnect();
        }
    }
}

void
CAprscClient::sendKeepAlive() {
    m_connector->OnTimeout([this](EErrc err_) { handleSendKeepAlive(err_); }, KEEPALIVE_INTERVAL);
}

void
CAprscClient::handleLogin(EErrc err_) {
    LockGuard lk(m_mutex);
    if (m_state == EState::CONNECTING) {
        if (err_ == EErrc::OK) {
            m_state = EState::RUNNING;
            m_backoff.Reset();
            logger.Info(LOG_PREFIX, "connected to ", m_endpoint.Host, ":", m_endpoint.Port);
            sendKeepAlive();
            read();
        } else {
            logger.Error(LOG_PREFIX, "send login failed");
            reconnect();
        }
    }
}

void
CAprscClient::handleSendKeepAlive(EErrc err_) {
    LockGuard lk(m_mutex);
    if (m_state == EState::RUNNING) {
        if (err_ == EErrc::OK) {
            m_connector->OnWrite("#keep-alive beacon\r\n", [this](EErrc err_) {
                LockGuard lk(m_mutex);
                if (m_state == EState::RUNNING) {
                    if (err_ != EErrc::OK) {
                        logger.Error(LOG_PREFIX, "send keep-alive beacon failed");
                        reconnect();
                    }
                }
            });
            sendKeepAlive();
        } else {
            sendKeepAlive();
        }
    }
}

auto
CAprscClient::logPrefix() const -> str {
    return LOG_PREFIX;
}
}  // namespace vfrb::client
