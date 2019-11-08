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

#include <functional>

#include "types.h"

namespace vfrb::client::net
{
struct Endpoint;

enum class ErrorCode : enum_type
{
    SUCCESS,
    FAILURE
};

using Callback     = std::function<void(ErrorCode)>;              ///< Common callback function
using ReadCallback = std::function<void(ErrorCode, Str const&)>;  ///< Callback function for read

/**
 * @brief The async TCP interface for clients
 *
 * As this is just an interface, all specific actions and details will be defined by a concrete
 * implementation. Hence all documentations placed here describe only an intention.
 */
class Connector
{
public:
    Connector()                   = default;
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
    virtual void onConnect(Endpoint const& endpoint, Callback const& callback) = 0;

    /**
     * @brief Attempt to read from current connection.
     * @param callback The callback to execute when done
     */
    virtual void onRead(ReadCallback const& callback) = 0;

    /**
     * @brief Attempt to write to current connection.
     * @param msg      The message to send
     * @param callback The callback to execute when done
     */
    virtual void onWrite(Str const& msg, Callback const& callback) = 0;

    /**
     * @brief Execute function after timeout.
     * @param callback The callback to execute
     * @param timeout  The timeout (default: 0)
     */
    virtual void onTimeout(Callback const& callback, u32 timeout = 0) = 0;

    /**
     * @brief Reset the timeout.
     * @param timeout The new timeout
     */
    virtual void resetTimer(u32 timeout) = 0;

    /**
     * @brief Check whether the timeout ran off.
     * @return true if it ran off, else false
     */
    virtual bool timerExpired() = 0;
};
}  // namespace vfrb::client::net
