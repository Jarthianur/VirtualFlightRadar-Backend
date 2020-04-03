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

#include "util/class_utils.hpp"

#include "CGuardedThread.hpp"

namespace vfrb::concurrent
{
/// A self joining thread group
class CThreadGroup
{
    NOT_COPYABLE(CThreadGroup)

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
