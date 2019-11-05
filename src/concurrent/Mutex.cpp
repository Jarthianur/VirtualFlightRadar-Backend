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

#include "concurrent/Mutex.h"

#include <utility>

namespace vfrb::concurrent
{
void Mutex::lock()
{
    m_mutex.lock();
}

void Mutex::unlock()
{
    m_mutex.unlock();
}

bool Mutex::try_lock()
{
    return m_mutex.try_lock();
}

Mutex const& Mutex::operator!() const
{
    return *this;
}

LockGuard::LockGuard(Mutex& mu) : m_lock(mu) {}

LockGuard::~LockGuard() noexcept {}

UniqueLock::UniqueLock(Mutex& mu) : m_lock(mu) {}

UniqueLock::UniqueLock(UniqueLock&& other) : m_lock(std::move(other.m_lock)) {}

UniqueLock::~UniqueLock() noexcept {}

void UniqueLock::lock()
{
    m_lock.lock();
}

void UniqueLock::unlock()
{
    m_lock.unlock();
}

UniqueLock& UniqueLock::operator=(UniqueLock&& other)
{
    m_lock = std::move(other.m_lock);
    return *this;
}
}  // namespace vfrb::concurrent
