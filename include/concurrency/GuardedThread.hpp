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

#include <future>
#include <stdexcept>
#include <thread>
#include <utility>

#include "util/defines.h"

namespace vfrb::concurrency
{
class GuardedThread
{
    NOT_COPYABLE(GuardedThread)

    std::thread       m_thread;
    std::future<void> m_state;

    template<typename FnT>
    [[gnu::always_inline]] void init(FnT&& fn)
    {
        auto task = std::packaged_task<void()>(std::forward<FnT>(fn));
        m_state   = task.get_future();
        m_thread  = std::thread(std::move(task));
    }

public:
    GuardedThread() = default;

    template<typename FnT>
    GuardedThread(FnT&& fn)
    {
        init<FnT>(std::forward<FnT>(fn));
    }

    ~GuardedThread() noexcept
    {
        try
        {
            m_state.get();
        }
        catch (...)
        {}
        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    GuardedThread(GuardedThread&& other)
    {
        if (m_state.valid())
        {
            throw std::logic_error("thread already running");
        }
        m_thread = std::move(other.m_thread);
        m_state  = std::move(other.m_state);
    }

    GuardedThread& operator=(GuardedThread&& other)
    {
        if (m_state.valid())
        {
            throw std::logic_error("thread already running");
        }
        m_thread = std::move(other.m_thread);
        m_state  = std::move(other.m_state);
        return *this;
    }

    template<typename FnT>
    void spawn(FnT&& fn)
    {
        if (m_state.valid())
        {
            throw std::logic_error("thread already running");
        }
        init<FnT>(std::forward<FnT>(fn));
    }
};
}  // namespace vfrb::concurrency
