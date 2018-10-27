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

#include <cstdint>
#include <functional>
#include <string>

namespace client
{
struct Endpoint;

/// @typedef Callback
/// Common callback function
using Callback = std::function<void(bool)>;

/// @typedef ReadCallback
/// Callback function for read
using ReadCallback = std::function<void(bool, const std::string&)>;

/**
 * @brief The async TCP interface for clients
 */
class Connector
{
public:
    /**
     * @brief Constructor
     */
    Connector() = default;

    /**
     * @brief Destructor
     */
    virtual ~Connector() noexcept = default;

    /**
     * @brief Run this connector.
     */
    virtual void run() = 0;

    /**
     * @brief Stop this connector.
     */
    virtual void stop() = 0;

    /**
     * @brief Close the connection.
     */
    virtual void close() = 0;

    /**
     * @brief Attempt to connect to endpoint.
     * @param endpoint The remote endpoint
     * @param callback The callback to execute when done
     */
    virtual void onConnect(const Endpoint& endpoint, const Callback& callback) = 0;

    /**
     * @brief Attempt to read from current connection.
     * @param callback The callback to execute when done
     */
    virtual void onRead(const ReadCallback& callback) = 0;

    /**
     * @brief Attempt to write to current connection.
     * @param msg      The message to send
     * @param callback The callback to execute when done
     */
    virtual void onWrite(const std::string& msg, const Callback& callback) = 0;

    /**
     * @brief Execute function after timeout.
     * @param callback The callback to execute
     * @param timeout  The timeout (default: 0)
     */
    virtual void onTimeout(const Callback& callback, std::uint32_t timeout = 0) = 0;

    /**
     * @brief Reset the timeout.
     * @param timeout The new timeout
     */
    virtual void resetTimer(std::uint32_t timeout) = 0;

    /**
     * @brief Check whether the timeout ran off.
     * @return true if it ran off, else false
     */
    virtual bool timerExpired() = 0;
};

}  // namespace client
