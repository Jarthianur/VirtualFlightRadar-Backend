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

#include "util/WorkerThread.h"

#include <chrono>
#include <utility>

namespace util
{
WorkerThread::WorkerThread(WorkerFn&& fn)
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

WorkerThread::~WorkerThread() noexcept
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

void WorkerThread::push(const std::string& data)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_workQ.push(data);
    m_cv.notify_one();
}

}  // namespace util
