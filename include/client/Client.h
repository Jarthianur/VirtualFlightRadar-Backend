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

#pragma once

#include <vector>

#include "concurrent/Mutex.hpp"
#include "net/Connector.hpp"
#include "net/Endpoint.hpp"
#include "util/class_utils.h"

#include "TimeoutBackoff.h"
#include "types.h"

namespace vfrb::feed
{
class Feed;
}  // namespace vfrb::feed
namespace vfrb::client
{
/**
 * @brief Base class for an async TCP client
 */
class IClient
{
    NOT_COPYABLE(IClient)

protected:
    enum class EState : enum_type
    {
        NONE,
        CONNECTING,
        RUNNING,
        STOPPING
    };

    concurrent::Mutex mutable m_mutex;
    CTimeoutBackoff               GUARDED_BY(m_mutex) m_backoff;
    SPtr<net::IConnector>         GUARDED_BY(m_mutex) m_connector;             /// Connector interface
    EState                        GUARDED_BY(m_mutex) m_state = EState::NONE;  /// Run state indicator
    net::SEndpoint const          m_endpoint;                                  /// Remote endpoint
    std::vector<SPtr<feed::Feed>> GUARDED_BY(m_mutex) m_feeds;  /// Container for subscribed feeds

    /**
     * @param endpoint  The connection Endpoint
     * @param component The component name
     * @param connector The Connector interface
     */
    IClient(net::SEndpoint const& endpoint_, SPtr<net::IConnector> connector_);

    /**
     * @brief Handler for connect
     * @param error The error indicator
     */
    virtual void handleConnect(net::EErrc error_) = 0;

    virtual char const* logPrefix() const = 0;

    /**
     * @brief Stop and close the connection.
     */
    virtual void stop() REQUIRES(m_mutex);

    /**
     * @brief Schedule to read from endpoint.
     */
    virtual void read() REQUIRES(m_mutex);

    /**
     * @brief Schedule to connect after some timeout.
     */
    void timedConnect() REQUIRES(m_mutex);

    /**
     * @brief Schedule to connect to endpoint.
     */
    void connect() REQUIRES(m_mutex);

    /**
     * @brief Close connection and schedule to connect after timeout.
     * @threadsafe
     */
    void reconnect() REQUIRES(m_mutex);

    /**
     * @brief Handler for timedConnect
     * @param error The error indicator
     */
    void handleTimedConnect(net::EErrc error_) REQUIRES(!m_mutex);

    /**
     * @brief Handler for read
     * @param error    The error indicator
     * @param response The received string
     */
    void handleRead(net::EErrc, Str const& response_) REQUIRES(!m_mutex);

public:
    virtual ~IClient() noexcept = default;

    /**
     * @brief Run the clients connection- and eventhandlers.
     * @note Returns after all queued handlers have returned.
     * @threadsafe
     */
    void Run() REQUIRES(!m_mutex);

    /**
     * @brief Stop after client has been started.
     * @note Wait until run has been called.
     * @threadsafe
     */
    void ScheduleStop() REQUIRES(!m_mutex);

    /**
     * @brief Subscribe a Feed to this client.
     * @param feed The Feed to subscribe
     * @threadsafe
     */
    void Subscribe(SPtr<feed::Feed> feed_) REQUIRES(!m_mutex);

    /**
     * @brief Compare to another client by value.
     * @param other The other Client
     * @return true if equals, else false
     */
    virtual bool Equals(IClient const& other_) const;

    /**
     * @brief Get a hash value of this client.
     * @return the hash value
     */
    virtual usize Hash() const;
};
}  // namespace vfrb::client
