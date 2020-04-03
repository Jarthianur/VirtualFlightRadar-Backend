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

#include "util/class_utils.hpp"

#include "types.hpp"

namespace vfrb::client::net
{
struct SEndpoint;

enum class EErrc : enum_type
{
    OK,
    Err
};

using Callback     = std::function<void(EErrc)>;                 ///< Common callback function
using ReadCallback = std::function<void(EErrc, String const&)>;  ///< Callback function for read

/**
 * @brief The async TCP interface for clients
 *
 * As this is just an interface, all specific actions and details will be defined by a concrete
 * implementation. Hence all documentations placed here describe only an intention.
 */
class IConnector
{
public:
    NOT_COPYABLE(IConnector)

    IConnector()                   = default;
    virtual ~IConnector() noexcept = default;

    /**
     * @brief Run this connector.
     */
    virtual void Run() = 0;

    /**
     * @brief Stop this connector.
     */
    virtual void Stop() = 0;

    /**
     * @brief Close the connection.
     */
    virtual void Close() = 0;

    /**
     * @brief Attempt to connect to endpoint.
     * @param endpoint The remote endpoint
     * @param callback The callback to execute when done
     */
    virtual void OnConnect(SEndpoint const& ep_, Callback const& cb_) = 0;

    /**
     * @brief Attempt to read from current connection.
     * @param callback The callback to execute when done
     */
    virtual void OnRead(ReadCallback const& cb_) = 0;

    /**
     * @brief Attempt to write to current connection.
     * @param msg      The message to send
     * @param callback The callback to execute when done
     */
    virtual void OnWrite(String const& str_, Callback const& cb_) = 0;

    /**
     * @brief Execute function after timeout.
     * @param callback The callback to execute
     * @param timeout  The timeout (default: 0)
     */
    virtual void OnTimeout(Callback const& cb_, u32 to_ = 0) = 0;

    /**
     * @brief Reset the timeout.
     * @param timeout The new timeout
     */
    virtual void ResetTimer(u32 to_) = 0;

    /**
     * @brief Check whether the timeout ran off.
     * @return true if it ran off, else false
     */
    virtual bool TimerExpired() = 0;
};
}  // namespace vfrb::client::net
