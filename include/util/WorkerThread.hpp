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
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

#include "util/defines.h"

namespace util
{
template<typename T>
class WorkerThread
{
    using WorkerFn = std::function<void(T&&)>;

    bool                    m_running;
    std::queue<T>           m_workQ;
    mutable std::mutex      m_mutex;
    std::condition_variable m_cv;
    std::thread             m_worker;

public:
    NOT_COPYABLE(WorkerThread)

    WorkerThread(WorkerFn&& fn);
    ~WorkerThread() noexcept;

    void push(const T& data);
};

template<typename T>
WorkerThread<T>::WorkerThread(WorkerFn&& fn)
    : m_running(false), m_worker([this, &fn] {
          std::unique_lock<std::mutex> lock(m_mutex);
          m_running = true;
          while (m_running)
          {
              m_cv.wait_for(lock, std::chrono::milliseconds(500));
              if (!m_running)
              {
                  break;
              }
              while (!m_workQ.empty())
              {
                  auto work = std::move(m_workQ.front());
                  m_workQ.pop();
                  lock.unlock();
                  fn(std::move(work));
                  lock.lock();
              }
          }
      })
{}

template<typename T>
WorkerThread<T>::~WorkerThread() noexcept
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (!m_workQ.empty())
        {
            m_workQ.pop();
        }
        m_running = false;
        m_cv.notify_one();
    }
    if (m_worker.joinable())
    {
        m_worker.join();
    }
}

template<typename T>
void WorkerThread<T>::push(const T& data)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_workQ.push(data);
    m_cv.notify_one();
}

}  // namespace util
