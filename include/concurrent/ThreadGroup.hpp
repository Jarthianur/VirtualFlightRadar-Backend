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

#include <list>

#include "util/class_utils.h"

#include "GuardedThread.hpp"

namespace vfrb::concurrent
{
class ThreadGroup
{
    NOT_COPYABLE(ThreadGroup)

    std::list<GuardedThread> m_threads;

public:
    ThreadGroup() = default;
    ~ThreadGroup() noexcept;

    template<typename FnT>
    void createThread(FnT&& fn);
    void joinAll();
};

inline ThreadGroup::~ThreadGroup() noexcept
{
    joinAll();
}

template<typename FnT>
void ThreadGroup::createThread(FnT&& fn)
{
    m_threads.push_back(GuardedThread(std::forward<FnT>(fn)));
}

inline void ThreadGroup::joinAll()
{
    m_threads.clear();
}
}  // namespace vfrb::concurrent
