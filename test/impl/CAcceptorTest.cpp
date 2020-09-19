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

#include "CAcceptorTest.hpp"

#include <thread>

#include "concurrent/Mutex.hpp"
#include "server/CConnection.hpp"

#include "CSocketTest.hpp"

using vfrb::concurrent::UniqueLock;
using vfrb::concurrent::LockGuard;

namespace vfrb::server::net
{
CAcceptorTest::CAcceptorTest() : m_sockets() {}

void
CAcceptorTest::Run() {
    UniqueLock lk(m_mutex);
    m_running = true;
    while (m_running) {
        lk.unlock();
        std::this_thread::yield();
        lk.lock();
    }
}

void
CAcceptorTest::Stop() {
    LockGuard lk(m_mutex);
    m_running = false;
    m_sockets.clear();
    m_stagedAddress.clear();
}

void
CAcceptorTest::OnAccept(Callback&& cb_) {
    m_onAcceptFn = std::move(cb_);
}

void
CAcceptorTest::Close() {
    m_sockets.pop_back();
}

auto
CAcceptorTest::StartConnection() -> CConnection<CSocketTest> {
    if (m_fail) {
        throw net::error::CSocketError("");
    }
    return CConnection<CSocketTest>(std::move(m_sockets.back().first));  // copy
}

auto
CAcceptorTest::StagedAddress() const -> String {
    return m_stagedAddress;
}

auto
CAcceptorTest::Connect(String const& addr_, bool failAccept_, bool failWrite_) -> usize {
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
    m_sockets.emplace_back(CSocketTest(addr_, buf, failWrite_), buf);
    m_onAcceptFn(failAccept_);  // false => no error
    return m_sockets.size() - 1;
}

auto
CAcceptorTest::Socket(usize i_) const -> CSocketTest const& {
    return m_sockets.at(i_).first;
}

auto
CAcceptorTest::Buffer(usize i_) const -> String const& {
    return *m_sockets.at(i_).second;
}

auto
CAcceptorTest::Sockets() const -> usize {
    return m_sockets.size();
}

void
CAcceptorTest::FailOnConnect(bool fail_) {
    m_fail = fail_;
}
}  // namespace vfrb::server::net
