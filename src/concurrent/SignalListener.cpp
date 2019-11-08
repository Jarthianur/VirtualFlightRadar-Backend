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

#include "concurrent/SignalListener.h"

namespace vfrb::concurrent
{
CSignalListener::CSignalListener() : m_ioService(), m_sigSet(m_ioService)
{
    m_sigSet.add(SIGINT);
    m_sigSet.add(SIGTERM);
#ifdef SIGQUIT
    m_sigSet.add(SIGQUIT);
#endif
}

CSignalListener::~CSignalListener() noexcept
{
    Stop();
}

void CSignalListener::Run()
{
    m_thread.Spawn([this]() { m_ioService.run(); });
}

void CSignalListener::Stop()
{
    if (!m_ioService.stopped())
    {
        m_ioService.stop();
    }
}

void CSignalListener::AddHandler(SignalHandler&& handler)
{
    m_sigSet.async_wait(std::move(handler));
}
}  // namespace vfrb::concurrent
