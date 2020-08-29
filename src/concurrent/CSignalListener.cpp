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

#include "concurrent/CSignalListener.hpp"

namespace vfrb::concurrent
{
CSignalListener::CSignalListener() : m_ioService(), m_sigSet(m_ioService) {
    m_sigSet.add(SIGINT);
    m_sigSet.add(SIGTERM);
#ifdef SIGQUIT
    m_sigSet.add(SIGQUIT);
#endif
}

CSignalListener::~CSignalListener() noexcept {
    Stop();
}

void
CSignalListener::Run() {
    m_thread.Spawn([this]() { m_ioService.run(); });
}

void
CSignalListener::Stop() {
    if (!m_ioService.stopped()) {
        m_ioService.stop();
    }
}

void
CSignalListener::AddHandler(SignalHandler&& handler_) {
    m_sigSet.async_wait(std::move(handler_));
}
}  // namespace vfrb::concurrent
