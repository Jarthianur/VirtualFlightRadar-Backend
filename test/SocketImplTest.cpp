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

#include "SocketImplTest.h"

#include "server/net/error/SocketError.h"

namespace vfrb
{
namespace server
{
namespace net
{
CSocketImplTest::CSocketImplTest(CSocketImplTest&& other_)
    : m_buffer(other_.m_buffer), m_address(other_.m_address), m_failWrite(other_.m_failWrite)
{}

CSocketImplTest& CSocketImplTest::operator=(CSocketImplTest&& other_)
{
    m_address   = other_.m_address;
    m_buffer    = other_.m_buffer;
    m_failWrite = other_.m_failWrite;
    return *this;
}

CSocketImplTest::CSocketImplTest(Str const& addr_, SPtr<Str> buf_, bool fail_)
    : m_buffer(buf_), m_address(addr_), m_failWrite(fail_)
{}

Str CSocketImplTest::Address() const
{
    return m_address;
}

bool CSocketImplTest::Write(StrView const& msg_)
{
    if (m_failWrite)
    {
        throw error::CSocketError();
    }
    m_buffer->clear();
    m_buffer->append(msg_);
    return true;
}

void CSocketImplTest::Close() {}
}  // namespace net
}  // namespace server
}  // namespace vfrb
