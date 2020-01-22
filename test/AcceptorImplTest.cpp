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
