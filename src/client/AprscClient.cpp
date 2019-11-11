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

#include "client/AprscClient.h"

#include <stdexcept>
#include <utility>

#include <boost/functional/hash.hpp>

#include "Logger.hpp"

using namespace vfrb::client::net;
using namespace vfrb::concurrent;

namespace vfrb::client
{
constexpr auto     LOG_PREFIX = "(AprscClient) ";
static auto const& logger     = CLogger::Instance();

CAprscClient::CAprscClient(SEndpoint const& ep_, Str const& login_, SPtr<IConnector> con_)
    : IClient(ep_, con_), m_login(login_ + "\r\n")
{}

bool CAprscClient::Equals(IClient const& other_) const
{
    try
    {
        auto const& other = dynamic_cast<CAprscClient const&>(other_);
        return IClient::Equals(other_) && this->m_login == other.m_login;
    }
    catch (std::bad_cast const&)
    {
        return false;
    }
}

usize CAprscClient::Hash() const
{
    usize seed = IClient::Hash();
    boost::hash_combine(seed, boost::hash_value(m_login));
    return seed;
}

void CAprscClient::handleConnect(EErrc err_)
{
    LockGuard lk(m_mutex);
    if (m_state == EState::CONNECTING)
    {
        if (err_ == EErrc::OK)
        {
            m_connector->OnWrite(m_login, std::bind(&CAprscClient::handleLogin, this, std::placeholders::_1));
        }
        else
        {
            logger.Warn(LOG_PREFIX, "failed to connect to ", m_endpoint.Host, ":", m_endpoint.Port);
            reconnect();
        }
    }
}

void CAprscClient::sendKeepAlive()
{
    m_connector->OnTimeout(std::bind(&CAprscClient::handleSendKeepAlive, this, std::placeholders::_1),
                           KEEPALIVE_INTERVAL);
}

void CAprscClient::handleLogin(EErrc err_)
{
    LockGuard lk(m_mutex);
    if (m_state == EState::CONNECTING)
    {
        if (err_ == EErrc::OK)
        {
            m_state = EState::RUNNING;
            m_backoff.Reset();
            logger.Info(LOG_PREFIX, "connected to ", m_endpoint.Host, ":", m_endpoint.Port);
            sendKeepAlive();
            read();
        }
        else
        {
            logger.Error(LOG_PREFIX, "send login failed");
            reconnect();
        }
    }
}

void CAprscClient::handleSendKeepAlive(EErrc err_)
{
    LockGuard lk(m_mutex);
    if (m_state == EState::RUNNING)
    {
        if (err_ == EErrc::OK)
        {
            m_connector->OnWrite("#keep-alive beacon\r\n", [this](EErrc err_) {
                LockGuard lk(m_mutex);
                if (m_state == EState::RUNNING)
                {
                    if (err_ != EErrc::OK)
                    {
                        logger.Error(LOG_PREFIX, "send keep-alive beacon failed");
                        reconnect();
                    }
                }
            });
            sendKeepAlive();
        }
        else
        {
            sendKeepAlive();
        }
    }
}

char const* CAprscClient::logPrefix() const
{
    return LOG_PREFIX;
}
}  // namespace vfrb::client
