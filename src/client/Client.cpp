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

#include "client/Client.h"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <utility>

#include <boost/functional/hash.hpp>

#include "feed/Feed.h"
#include "util/Logger.hpp"

static auto const& logger = vfrb::CLogger::Instance();

using namespace vfrb::client::net;
using namespace vfrb::concurrent;

namespace vfrb::client
{
IClient::IClient(SEndpoint const& endpoint, SPtr<IConnector> connector)
    : m_connector(connector), m_endpoint(endpoint)
{}

bool IClient::Equals(IClient const& other) const
{
    return this->m_endpoint == other.m_endpoint;
}

usize IClient::Hash() const
{
    usize seed = 0;
    boost::hash_combine(seed, boost::hash_value(m_endpoint.Host));
    boost::hash_combine(seed, boost::hash_value(m_endpoint.Port));
    return seed;
}

void IClient::subscribe(SPtr<feed::Feed> feed)
{
    LockGuard lk(m_mutex);
    m_feeds.push_back(feed);
    std::sort(m_feeds.begin(), m_feeds.end(), [](SPtr<feed::Feed> const& f1, SPtr<feed::Feed> const& f2) {
        return f1->priority() > f2->priority();
    });
}

void IClient::run()
{
    UniqueLock lk(m_mutex);
    if (m_state == EState::NONE)
    {
        connect();
        lk.unlock();
        m_connector->run();
        m_state = EState::NONE;
    }
}

void IClient::connect()
{
    m_state = EState::CONNECTING;
    m_connector->onConnect(m_endpoint, std::bind(&IClient::handleConnect, this, std::placeholders::_1));
}

void IClient::reconnect()
{
    if (m_state != EState::STOPPING)
    {
        m_state = EState::CONNECTING;
        logger.info(logPrefix(), "schedule reconnect to ", m_endpoint.Host, ":", m_endpoint.Port);
        m_connector->close();
        timedConnect();
    }
}

void IClient::timedConnect()
{
    m_connector->onTimeout(std::bind(&IClient::handleTimedConnect, this, std::placeholders::_1),
                           m_backoff.next());
}

void IClient::stop()
{
    if (m_state == EState::RUNNING || m_state == EState::CONNECTING)
    {
        m_state = EState::STOPPING;
        logger.info(logPrefix(), "disconnect from ", m_endpoint.Host, ":", m_endpoint.Port);
        m_connector->stop();
    }
}

void IClient::scheduleStop()
{
    std::condition_variable_any cond_ready;
    UniqueLock                  lk(m_mutex);
    cond_ready.wait_for(lk, std::chrono::milliseconds(100), [this] { return m_state != EState::NONE; });
    stop();
}

void IClient::Read()
{
    m_connector->onRead(std::bind(&IClient::handleRead, this, std::placeholders::_1, std::placeholders::_2));
}

void IClient::handleTimedConnect(EErrc error)
{
    if (error == EErrc::SUCCESS)
    {
        LockGuard lk(m_mutex);
        logger.info(logPrefix(), "try connect to ", m_endpoint.Host, ":", m_endpoint.Port);
        connect();
    }
    else
    {
        logger.error(logPrefix(), "failed to connect after timeout");
        scheduleStop();
    }
}

void IClient::handleRead(EErrc error, Str const& response)
{
    LockGuard lk(m_mutex);
    if (m_state == EState::RUNNING)
    {
        if (error == EErrc::SUCCESS)
        {
            for (auto& it : m_feeds)
            {
                if (!it->process(response))
                {
                    stop();
                }
            }
            Read();
        }
        else
        {
            logger.error(logPrefix(), "failed to read message");
            reconnect();
        }
    }
}
}  // namespace vfrb::client
