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
#include <mutex>
#include <thread>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "defines.h"

namespace util
{
/// @typedef SignalHandler
/// Handler function
using SignalHandler = std::function<void(const boost::system::error_code&, const int)>;

/**
 * @brief Catch and handle system signals.
 */
class SignalListener
{
public:
    NOT_COPYABLE(SignalListener)

    /**
     * @brief Constructor
     */
    SignalListener();

    /**
     * @brief Destructor
     */
    ~SignalListener() noexcept;

    /**
     * @brief Run this signal listener.
     * @threadsafe
     */
    void run();

    /**
     * @brief Stop this signal listener.
     * @threadsafe
     */
    void stop();

    /**
     * @brief Add a SignalHandler.
     * @param handler The handler to invoke when signal caught
     * @threadsafe
     */
    void addHandler(const SignalHandler& handler);

private:
    /// Internal IO-service
    boost::asio::io_service m_ioService;

    /// Internal signal set
    boost::asio::signal_set m_sigSet;

    /// Thread to run this
    std::thread m_thread;

    /// Mutex for threadsafety
    mutable std::mutex m_mutex;
};
}  // namespace util
