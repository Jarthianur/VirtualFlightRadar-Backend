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

#include <list>

#include "util/ClassUtils.hpp"

#include "CGuardedThread.hpp"

namespace vfrb::concurrent
{
/// A self joining thread group
class CThreadGroup
{
    NOT_COPYABLE(CThreadGroup)
    NOT_MOVABLE(CThreadGroup)

    std::list<CGuardedThread> m_threads;  ///< The grouped threads

public:
    CThreadGroup() = default;
    ~CThreadGroup() noexcept;

    /**
     * Create and put a thread into this group.
     * @param fn_ The function for the thread to run
     */
    template<typename FnT>
    void CreateThread(FnT&& fn_);

    /// Clear the thread group and join all threads.
    void JoinAll();
};

inline CThreadGroup::~CThreadGroup() noexcept {
    JoinAll();
}

template<typename FnT>
void CThreadGroup::CreateThread(FnT&& fn_) {
    m_threads.push_back(CGuardedThread(std::forward<FnT>(fn_)));
}

inline void CThreadGroup::JoinAll() {
    m_threads.clear();
}
}  // namespace vfrb::concurrent
