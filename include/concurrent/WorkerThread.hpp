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

#include "util/class_utils.h"

#include "GuardedThread.hpp"
#include "types.h"

namespace vfrb::concurrent
{
template<typename DataT>
class WorkerThread
{
    NOT_COPYABLE(WorkerThread)

    bool              m_running;
    std::queue<DataT> m_workQ;
    std::mutex mutable m_mutex;
    std::condition_variable m_cv;
    GuardedThread           m_worker;

public:
    template<typename FnT>
    explicit WorkerThread(FnT&& fn);
    ~WorkerThread() noexcept;

    void push(DataT&& data);
};

template<typename DataT>
template<typename FnT>
WorkerThread<DataT>::WorkerThread(FnT&& fn)
    : m_running(false), m_worker([this, fn = std::forward<FnT>(fn)] {
          std::unique_lock lk(m_mutex);
          m_running = true;
          while (m_running)
          {
              m_cv.wait_for(lk, std::chrono::milliseconds(500));
              if (!m_running)
              {
                  break;
              }
              while (!m_workQ.empty())
              {
                  auto work = std::move(m_workQ.front());
                  m_workQ.pop();
                  lk.unlock();
                  std::invoke(fn, std::move(work));
                  lk.lock();
              }
          }
      })
{}

template<typename DataT>
WorkerThread<DataT>::~WorkerThread() noexcept
{
    std::lock_guard lk(m_mutex);
    while (!m_workQ.empty())
    {
        m_workQ.pop();
    }
    m_running = false;
    m_cv.notify_one();
}

template<typename DataT>
void WorkerThread<DataT>::push(DataT&& data)
{
    std::lock_guard lk(m_mutex);
    m_workQ.push(std::move(data));
    m_cv.notify_one();
}
}  // namespace vfrb::concurrent
