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

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "util/defines.h"

namespace util
{
class WorkerThread
{
    using WorkerFn = std::function<void(std::string&&)>;

    bool                    m_running;
    std::queue<std::string> m_workQ;
    mutable std::mutex      m_mutex;
    std::condition_variable m_cv;
    std::thread             m_worker;

public:
    NOT_COPYABLE(WorkerThread)

    WorkerThread(WorkerFn&& fn);
    ~WorkerThread() noexcept;

    void push(const std::string& data);
};

}  // namespace util
