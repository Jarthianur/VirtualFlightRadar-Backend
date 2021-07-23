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

#include <chrono>
#include <condition_variable>
#include <functional>
#include <queue>

#include "util/ClassUtils.hpp"

#include "CGuardedThread.hpp"
#include "Mutex.hpp"
#include "Types.hpp"

namespace vfrb::concurrent
{
template<typename DataT>
class CWorkerThread
{
    NOT_COPYABLE(CWorkerThread)
    NOT_MOVABLE(CWorkerThread)

    CTCONST POLL_TIME = 200;

    Mutex mutable m_mutex;
    std::condition_variable_any GUARDED_BY(m_mutex) m_cv;
    bool                        GUARDED_BY(m_mutex) m_idle;
    bool                        GUARDED_BY(m_mutex) m_running;
    std::queue<DataT>           GUARDED_BY(m_mutex) m_workQ;
    CGuardedThread              m_worker;

public:
    template<typename FnT>
    explicit CWorkerThread(FnT&& fn_);
    ~CWorkerThread() noexcept;

    void
    Put(DataT&& data_) REQUIRES(!m_mutex);

    auto REQUIRES(!m_mutex) Idle() const -> bool;
};

template<typename DataT>
template<typename FnT>
CWorkerThread<DataT>::CWorkerThread(FnT&& fn_)
    : m_idle(true), m_running(true), m_worker([this, fn = std::forward<FnT>(fn_)]() EXCLUDES(m_mutex) {
          MutableLock lk(m_mutex);
          while (m_running) {
              m_cv.wait_for(lk, std::chrono::milliseconds(POLL_TIME),
                            [this]() NO_THREAD_SAFETY_ANALYSIS { return !m_running; });
              if (!m_running) {
                  break;
              }
              while (!m_workQ.empty()) {
                  auto work = std::move(m_workQ.front());
                  m_workQ.pop();
                  lk.unlock();
                  std::invoke(fn, std::move(work));
                  lk.lock();
              }
              m_idle = true;
          }
      }) {}

template<typename DataT>
CWorkerThread<DataT>::~CWorkerThread() noexcept {
    ImmutableLock lk(m_mutex);
    while (!m_workQ.empty()) {
        m_workQ.pop();
    }
    m_running = false;
    m_cv.notify_all();
}

template<typename DataT>
void
CWorkerThread<DataT>::Put(DataT&& data_) REQUIRES(!m_mutex) {
    ImmutableLock lk(m_mutex);
    m_workQ.push(std::move(data_));
    m_idle = false;
    m_cv.notify_all();
}

template<typename DataT>
auto
CWorkerThread<DataT>::Idle() const -> bool {
    ImmutableLock lk(m_mutex);
    return m_idle;
}
}  // namespace vfrb::concurrent
