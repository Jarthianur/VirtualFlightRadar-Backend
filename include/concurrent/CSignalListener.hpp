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

#include "util/class_utils.hpp"

#include "GuardedThread.hpp"
#include "types.hpp"

namespace vfrb::concurrent
{
using SignalHandler = std::function<void(boost::system::error_code const&, int const)>;

/// Catch and handle system signals.
class CSignalListener
{
    NOT_COPYABLE(CSignalListener)

    boost::asio::io_service m_ioService;  ///< Internal IO-service
    boost::asio::signal_set m_sigSet;     ///< Internal signal set
    CGuardedThread          m_thread;     ///< The underlying thread

public:
    CSignalListener();
    ~CSignalListener() noexcept;

    void Run();
    void Stop();

    /**
     * Add a handler function to be executed when a signal is caught.
     * @param handler_ The handler to invoke
     */
    void AddHandler(SignalHandler&& handler_);
};
}  // namespace vfrb::concurrent
