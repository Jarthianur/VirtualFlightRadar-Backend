/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <functional>

#include "util/ClassUtils.hpp"

#include "CGuardedThread.hpp"
#include "Types.hpp"
#include "asio.hpp"

namespace vfrb::concurrent
{
using SignalHandler = std::function<void(asio::error_code const&, int const)>;

class CSignalListener
{
    NOT_COPYABLE(CSignalListener)
    NOT_MOVABLE(CSignalListener)

    asio::io_service m_ioCtx;
    asio::signal_set m_sigSet;
    CGuardedThread   m_thread;

public:
    CSignalListener();
    ~CSignalListener() noexcept;

    void
    Run();

    void
    Stop();

    void
    AddHandler(SignalHandler&& handler_);
};
}  // namespace vfrb::concurrent
