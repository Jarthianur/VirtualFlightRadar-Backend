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

#include "CAcceptorImplTest.hpp"

#include <thread>

#include "concurrent/Mutex.hpp"
#include "server/CConnection.hpp"

#include "CSocketImplTest.hpp"

using vfrb::concurrent::UniqueLock;
using vfrb::concurrent::LockGuard;

namespace vfrb::server::net
{
CAcceptorImplTest::CAcceptorImplTest() : m_sockets() {}

void CAcceptorImplTest::Run() {
    UniqueLock lk(m_mutex);
    m_running = true;
    while (m_running) {
        lk.unlock();
        std::this_thread::yield();
        lk.lock();
    }
}

void CAcceptorImplTest::Stop() {
    LockGuard lk(m_mutex);
    m_running = false;
    m_sockets.clear();
    m_stagedAddress.clear();
}

void CAcceptorImplTest::OnAccept(Callback&& cb_) {
    m_onAcceptFn = std::move(cb_);
}

void CAcceptorImplTest::Close() {
    m_sockets.pop_back();
}

auto CAcceptorImplTest::StartConnection() -> CConnection<CSocketImplTest> {
    if (m_fail) {
        throw net::error::CSocketError("");
    }
    return CConnection<CSocketImplTest>(std::move(m_sockets.back().first));  // copy
}

auto CAcceptorImplTest::StagedAddress() const -> String {
    return m_stagedAddress;
}

auto CAcceptorImplTest::Connect(String const& addr_, bool failAccept_, bool failWrite_) -> usize {
    {
        UniqueLock lk(m_mutex);
        while (!m_running) {
            lk.unlock();
            std::this_thread::yield();
            lk.lock();
        }
    }
    m_stagedAddress = addr_;
    auto buf        = std::make_shared<String>("");
    m_sockets.emplace_back(CSocketImplTest(addr_, buf, failWrite_), buf);
    m_onAcceptFn(failAccept_);  // false => no error
    return m_sockets.size() - 1;
}

auto CAcceptorImplTest::Socket(usize i_) const -> CSocketImplTest const& {
    return m_sockets.at(i_).first;
}

auto CAcceptorImplTest::Buffer(usize i_) const -> String const& {
    return *m_sockets.at(i_).second;
}

auto CAcceptorImplTest::Sockets() const -> usize {
    return m_sockets.size();
}

void CAcceptorImplTest::FailOnConnect(bool fail_) {
    m_fail = fail_;
}
}  // namespace vfrb::server::net
