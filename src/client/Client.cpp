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

#include <chrono>
#include <condition_variable>
#include <utility>

#include <boost/functional/hash.hpp>

#include "feed/Feed.h"
#include "util/Logger.hpp"

namespace client
{
using namespace net;

Client::Client(const Endpoint& endpoint, const char* component,
               std::shared_ptr<Connector> connector)
    : m_connector(connector), m_component(component), m_endpoint(endpoint)
{}

void Client::run()
{
    std::unique_lock<std::mutex> lk(m_mutex);
    if (m_state == State::NONE)
    {
        connect();
        lk.unlock();
        m_connector->run();
        m_state = State::NONE;
    }
}

bool Client::equals(const Client& other) const
{
    return this->m_endpoint == other.m_endpoint;
}

std::size_t Client::hash() const
{
    std::size_t seed = 0;
    boost::hash_combine(seed, boost::hash_value(m_endpoint.host));
    boost::hash_combine(seed, boost::hash_value(m_endpoint.port));
    return seed;
}

void Client::subscribe(std::shared_ptr<feed::Feed> feed)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_feeds.push_back(feed);
}

void Client::connect()
{
    m_state = State::CONNECTING;
    m_connector->onConnect(m_endpoint,
                           std::bind(&Client::handleConnect, this, std::placeholders::_1));
}

void Client::reconnect()
{
    if (m_state != State::STOPPING)
    {
        m_state = State::CONNECTING;
        logger.info(m_component, " schedule reconnect to ", m_endpoint.host, ":", m_endpoint.port);
        m_connector->close();
        timedConnect();
    }
}

void Client::timedConnect()
{
    m_connector->onTimeout(std::bind(&Client::handleTimedConnect, this, std::placeholders::_1),
                           C_CON_WAIT_TIMEVAL);
}

void Client::stop()
{
    if (m_state == State::RUNNING || m_state == State::CONNECTING)
    {
        m_state = State::STOPPING;
        logger.info(m_component, " disconnect from ", m_endpoint.host, ":", m_endpoint.port);
        m_connector->stop();
    }
}

void Client::scheduleStop()
{
    std::condition_variable      cond_ready;
    std::unique_lock<std::mutex> lk(m_mutex);
    cond_ready.wait_for(lk, std::chrono::milliseconds(100),
                        [this] { return m_state != State::NONE; });
    stop();
}

void Client::read()
{
    m_connector->onRead(
        std::bind(&Client::handleRead, this, std::placeholders::_1, std::placeholders::_2));
}

void Client::handleTimedConnect(ErrorCode error)
{
    if (error == ErrorCode::SUCCESS)
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        logger.info(m_component, " try connect to ", m_endpoint.host, ":", m_endpoint.port);
        connect();
    }
    else
    {
        logger.error(m_component, " failed to connect after timeout");
        scheduleStop();
    }
}

void Client::handleRead(ErrorCode error, const std::string& response)
{
    std::lock_guard<std::mutex> lk(m_mutex);
    if (m_state == State::RUNNING)
    {
        if (error == ErrorCode::SUCCESS)
        {
            for (auto& it : m_feeds)
            {
                if (!it->process(response))
                {
                    stop();
                }
            }
            read();
        }
        else
        {
            logger.error(m_component, " failed to read message");
            reconnect();
        }
    }
}
}  // namespace client
