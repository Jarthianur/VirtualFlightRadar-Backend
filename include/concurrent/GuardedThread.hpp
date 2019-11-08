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
#include <thread>
#include <utility>

#include "error/Error.hpp"
#include "util/class_utils.h"

namespace vfrb::concurrent
{
class GuardedThread
{
    NOT_COPYABLE(GuardedThread)

    std::thread       m_thread;
    std::future<void> m_state;

    template<typename FnT>
    void init(FnT&& fn_);

public:
    GuardedThread() = default;

    template<typename FnT>
    explicit GuardedThread(FnT&& fn_);

    ~GuardedThread() noexcept;

    GuardedThread(GuardedThread&& other_);

    GuardedThread& operator=(GuardedThread&& other_);

    template<typename FnT>
    void Spawn(FnT&& fn_);
};

namespace error
{
class ThreadUsedError : public vfrb::error::Error
{
public:
    ThreadUsedError()                    = default;
    ~ThreadUsedError() noexcept override = default;

    char const* what() const noexcept override
    {
        return "thread already used";
    }
};
}  // namespace error

template<typename FnT>
[[gnu::always_inline]] inline void GuardedThread::init(FnT&& fn_)
{
    auto task = std::packaged_task<void()>(std::forward<FnT>(fn_));
    m_state   = task.get_future();
    m_thread  = std::thread(std::move(task));
}

template<typename FnT>
GuardedThread::GuardedThread(FnT&& fn_)
{
    init<FnT>(std::forward<FnT>(fn_));
}

inline GuardedThread::~GuardedThread() noexcept
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

inline GuardedThread::GuardedThread(GuardedThread&& other_)
{
    if (m_state.valid())
    {
        throw error::ThreadUsedError();
    }
    m_thread = std::move(other_.m_thread);
    m_state  = std::move(other_.m_state);
}

inline GuardedThread& GuardedThread::operator=(GuardedThread&& other_)
{
    if (m_state.valid())
    {
        throw error::ThreadUsedError();
    }
    m_thread = std::move(other_.m_thread);
    m_state  = std::move(other_.m_state);
    return *this;
}

template<typename FnT>
void GuardedThread::Spawn(FnT&& fn_)
{
    if (m_state.valid())
    {
        throw error::ThreadUsedError();
    }
    init<FnT>(std::forward<FnT>(fn_));
}
}  // namespace vfrb::concurrent
