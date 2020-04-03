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

#include <chrono>
#include <condition_variable>
#include <functional>
#include <queue>

#include "util/class_utils.hpp"

#include "CGuardedThread.hpp"
#include "Mutex.hpp"
#include "types.hpp"

namespace vfrb::concurrent
{
/**
 * A thread that executes a function on elements in a work queue.
 * @tparam DataT The element type of the work queue
 */
template<typename DataT>
class CWorkerThread
{
    NOT_COPYABLE(CWorkerThread)

    inline static constexpr auto const POLL_TIME = 200;

    Mutex mutable m_mutex;
    std::condition_variable_any GUARDED_BY(m_mutex) m_cv;       ///< Wait and notify for work
    bool                        GUARDED_BY(m_mutex) m_running;  ///< Is thread running?
    std::queue<DataT>           GUARDED_BY(m_mutex) m_workQ;    ///< The work queue
    CGuardedThread              m_worker;                       ///< The underlying worker thread

public:
    /// @param fn_ The function to execute for each element
    template<typename FnT>
    explicit CWorkerThread(FnT&& fn_);
    ~CWorkerThread() noexcept;

    /**
     * Push an element into the work queue.
     * @param data_ The element to push
     */
    void Push(DataT&& data_) REQUIRES(!m_mutex);
};

template<typename DataT>
template<typename FnT>
CWorkerThread<DataT>::CWorkerThread(FnT&& fn_)
    : m_running(false), m_worker([this, fn = std::forward<FnT>(fn_)]() EXCLUDES(m_mutex) {
          UniqueLock lk(m_mutex);
          m_running = true;
          while (m_running) {
              m_cv.wait_for(lk, std::chrono::milliseconds(POLL_TIME));
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
          }
      }) {}

template<typename DataT>
CWorkerThread<DataT>::~CWorkerThread() noexcept {
    LockGuard lk(m_mutex);
    while (!m_workQ.empty()) {
        m_workQ.pop();
    }
    m_running = false;
    m_cv.notify_one();
}

template<typename DataT>
void CWorkerThread<DataT>::Push(DataT&& data_) REQUIRES(!m_mutex) {
    LockGuard lk(m_mutex);
    m_workQ.push(std::move(data_));
    m_cv.notify_one();
}
}  // namespace vfrb::concurrent
