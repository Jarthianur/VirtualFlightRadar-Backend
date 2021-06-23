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

#include "concurrent/CGuardedThread.hpp"
#include "concurrent/Mutex.hpp"
#include "util/ClassUtils.hpp"

#include "Types.hpp"

namespace vfrb::util
{
template<typename KeyT, typename ValT>
class CHashQueue
{
    GUARDED_BY(m_mutex) HashMap<KeyT, ValT> m_data;
    concurrent::Mutex mutable m_mutex;
    GUARDED_BY(m_mutex) bool m_running;
    std::condition_variable_any m_cond;
    concurrent::CGuardedThread  m_thd;

public:
    using Iterator = typename decltype(m_data)::iterator;

    template<typename FnT>
    CHashQueue(u32 sec_, FnT&& clean_);
    ~CHashQueue() noexcept;

    template<typename InT, typename FoundFnT, typename NotFoundFnT>
    REQUIRES(!m_mutex)
    void Push(InT const& item_, FoundFnT&& foundFn_, NotFoundFnT&& notFoundFn_);

    template<typename FnT>
    void
    RemoveIf(FnT&& pred_);
};

template<typename KeyT, typename ValT>
template<typename InT, typename FoundFnT, typename NotFoundFnT>
void
CHashQueue<KeyT, ValT>::Push(InT const& item_, FoundFnT&& foundFn_, NotFoundFnT&& notFoundFn_) {
    concurrent::ImmutableLock lk(m_mutex);
    Iterator                  it{m_data.find(item_.first)};
    if (it != m_data.end()) {
        foundFn_(it, item_);
    } else {
        m_data.emplace(item_.first, notFoundFn_(item_));
    }
}

template<typename KeyT, typename ValT>
template<typename FnT>
CHashQueue<KeyT, ValT>::CHashQueue(u32 sec_, FnT&& clean_)
    : m_running(true), m_thd([this, &clean_, sec_]() NO_THREAD_SAFETY_ANALYSIS {
          concurrent::MutableLock lk(m_mutex);
          while (m_running) {
              m_cond.wait_for(lk, std::chrono::seconds(sec_),
                              [this]() NO_THREAD_SAFETY_ANALYSIS { return !m_running; });
              if (!m_running) {
                  break;
              }
              lk.unlock();
              clean_();
              lk.lock();
          }
      }) {}

template<typename KeyT, typename ValT>
CHashQueue<KeyT, ValT>::~CHashQueue() noexcept {
    concurrent::ImmutableLock lk(m_mutex);
    m_running = false;
    m_cond.notify_all();
}

template<typename KeyT, typename ValT>
template<typename FnT>
void
CHashQueue<KeyT, ValT>::RemoveIf(FnT&& pred_) {
    concurrent::ImmutableLock lk(m_mutex);
    for (auto it = m_data.begin(); it != m_data.end();) {
        if (pred_(it)) {
            it = m_data.erase(it);
        } else {
            ++it;
        }
    }
}
}  // namespace vfrb::util
