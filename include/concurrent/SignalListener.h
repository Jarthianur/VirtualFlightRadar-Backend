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

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "util/class_utils.h"

#include "GuardedThread.hpp"
#include "types.h"

namespace vfrb::concurrent
{
/// Handler function
using SignalHandler = std::function<void(boost::system::error_code const&, int const)>;

/**
 * @brief Catch and handle system signals.
 */
class SignalListener
{
    NOT_COPYABLE(SignalListener)

    boost::asio::io_service m_ioService;  ///< Internal IO-service
    boost::asio::signal_set m_sigSet;     ///< Internal signal set
    GuardedThread           m_thread;     ///< Thread to run this

public:
    SignalListener();
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
    void addHandler(SignalHandler&& handler);
};
}  // namespace vfrb::concurrent
