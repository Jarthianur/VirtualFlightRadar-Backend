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

#include "error/IError.hpp"

namespace vfrb::concurrent
{
/// RAII style, self joining thread
class CGuardedThread
{
    std::thread       m_thread;  ///< The underlying thread
    std::future<void> m_state;   ///< The run state, if valid then is thread running

    /**
     * Initialize this thread with a function.
     * @param fn_ The function to run
     */
    template<typename FnT>
    void init(FnT&& fn_);

public:
    CGuardedThread() = default;

    CGuardedThread(CGuardedThread const&) = delete;

    /// @param fn_ The function to run
    template<typename FnT>
    explicit CGuardedThread(FnT&& fn_);

    ~CGuardedThread() noexcept;

    CGuardedThread(CGuardedThread&& other_) noexcept;

    CGuardedThread& operator=(CGuardedThread&& other_) noexcept;

    CGuardedThread& operator=(CGuardedThread const&) = delete;

    /**
     * Spawn this thread with given function if not done on construction yet.
     * @param fn_ The function to run
     * @throw vfrb::concurrent::error::CThreadUsedError
     */
    template<typename FnT>
    void Spawn(FnT&& fn_);
};

namespace error
{
/// Error to indicate that a thread is already running.
class CThreadUsedError : public vfrb::error::IError
{
public:
    char const* Message() const noexcept override {
        return "thread already used";
    }
};
}  // namespace error

template<typename FnT>
[[gnu::always_inline]] inline void CGuardedThread::init(FnT&& fn_) {
    auto task = std::packaged_task<void()>(std::forward<FnT>(fn_));
    m_state   = task.get_future();
    m_thread  = std::thread(std::move(task));
}

template<typename FnT>
CGuardedThread::CGuardedThread(FnT&& fn_) {
    init<FnT>(std::forward<FnT>(fn_));
}

inline CGuardedThread::~CGuardedThread() noexcept {
    try {
        m_state.get();
    } catch (...) {
    }
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

inline CGuardedThread::CGuardedThread(CGuardedThread&& other_) noexcept {
    m_thread = std::move(other_.m_thread);
    m_state  = std::move(other_.m_state);
}

inline CGuardedThread& CGuardedThread::operator=(CGuardedThread&& other_) noexcept {
    m_thread = std::move(other_.m_thread);
    m_state  = std::move(other_.m_state);
    return *this;
}

template<typename FnT>
void CGuardedThread::Spawn(FnT&& fn_) {
    if (m_state.valid()) {
        throw error::CThreadUsedError();
    }
    init<FnT>(std::forward<FnT>(fn_));
}
}  // namespace vfrb::concurrent
