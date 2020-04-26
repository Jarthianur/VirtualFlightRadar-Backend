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

#include "AcceptorImplTest.h"

#include <thread>

#include "server/Connection.hpp"

#include "SocketImplTest.h"

namespace vfrb
{
namespace server
{
namespace net
{
CAcceptorImplTest::CAcceptorImplTest(bool failConnect_) : m_fail(failConnect_) {}

void CAcceptorImplTest::Run()
{
    while (!m_stopped)
    {
        std::this_thread::yield();
    }
    m_sockets.clear();
}

void CAcceptorImplTest::Stop()
{
    m_stopped = true;
}

void CAcceptorImplTest::OnAccept(Callback&& cb_)
{
    m_onAcceptFn = std::move(cb_);
}

void CAcceptorImplTest::Close()
{
    m_sockets.pop_back();
}

CConnection<CSocketImplTest> CAcceptorImplTest::StartConnection()
{
    if (m_fail)
    {
        throw net::error::CSocketError();
    }
    return CConnection<CSocketImplTest>(std::move(m_sockets.back().first));  // copy
}

Str CAcceptorImplTest::StagedAddress() const
{
    return m_stagedAddress;
}

usize CAcceptorImplTest::Connect(Str const& addr_, bool failAccept_, bool failWrite_)
{
    m_stagedAddress = addr_;
    auto buf        = std::make_shared<Str>("");
    m_sockets.push_back(std::make_pair(CSocketImplTest(addr_, buf, failWrite_), buf));
    m_onAcceptFn(failAccept_);  // false => no error
    return m_sockets.size() - 1;
}

auto CAcceptorImplTest::Socket(usize i_) const -> CSocketImplTest const&
{
    return m_sockets.at(i_).first;
}

auto CAcceptorImplTest::Buffer(usize i_) const -> Str const&
{
    return *m_sockets.at(i_).second;
}

usize CAcceptorImplTest::Sockets() const
{
    return m_sockets.size();
}
}  // namespace net
}  // namespace server
}  // namespace vfrb
