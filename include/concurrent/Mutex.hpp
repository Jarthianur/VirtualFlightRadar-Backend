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

/**
 * @see https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#mutexheader
 */

#pragma once

#include <mutex>

#include "util/class_utils.h"

#if defined(__clang__) && (!defined(SWIG))
#    define THREAD_ANNOTATION_ATTRIBUTE__(x) __attribute__((x))
#else
#    define THREAD_ANNOTATION_ATTRIBUTE__(x)  // no-op
#endif

#define CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(capability(x))

#define SCOPED_CAPABILITY THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)

#define GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

#define PT_GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))

#define ACQUIRED_BEFORE(...) THREAD_ANNOTATION_ATTRIBUTE__(acquired_before(__VA_ARGS__))

#define ACQUIRED_AFTER(...) THREAD_ANNOTATION_ATTRIBUTE__(acquired_after(__VA_ARGS__))

#define REQUIRES(...) THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))

#define ACQUIRE(...) THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))

#define RELEASE(...) THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

#define TRY_ACQUIRE(...) THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_capability(__VA_ARGS__))

#define EXCLUDES(...) THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

#define ASSERT_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(assert_capability(x))

#define RETURN_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(lock_returned(x))

#define NO_THREAD_SAFETY_ANALYSIS THREAD_ANNOTATION_ATTRIBUTE__(no_thread_safety_analysis)

namespace vfrb::concurrent
{
#if defined(__clang__) && (!defined(SWIG))

class CAPABILITY("mutex") Mutex
{
    NOT_COPYABLE(Mutex)

    std::mutex m_mutex;

public:
    Mutex()           = default;
    ~Mutex() noexcept = default;

    void lock() ACQUIRE()
    {
        m_mutex.lock();
    }

    void unlock() RELEASE()
    {
        m_mutex.unlock();
    }

    bool try_lock() TRY_ACQUIRE(true)
    {
        return m_mutex.try_lock();
    }

    Mutex const& operator!() const
    {
        return *this;
    }
};

class SCOPED_CAPABILITY LockGuard
{
    NOT_COPYABLE(LockGuard)

    std::lock_guard<Mutex> m_lock;

public:
    LockGuard(Mutex& mu_) ACQUIRE(mu_) : m_lock(mu_) {}
    ~LockGuard() noexcept RELEASE() {}
};

#else

using Mutex     = std::mutex;
using LockGuard = std::lock_guard<std::mutex>;

#endif

using UniqueLock = std::unique_lock<Mutex>;
}  // namespace vfrb::concurrent
