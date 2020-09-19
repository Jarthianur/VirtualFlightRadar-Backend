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

#include "CSocketTest.hpp"

#include "server/net/error/CSocketError.hpp"

namespace vfrb::server::net
{
CSocketTest::CSocketTest(CSocketTest&& other_) noexcept
    : m_buffer(other_.m_buffer), m_address(other_.m_address), m_failWrite(other_.m_failWrite) {}

auto
CSocketTest::operator=(CSocketTest&& other_) noexcept -> CSocketTest& {
    m_address   = other_.m_address;
    m_buffer    = other_.m_buffer;
    m_failWrite = other_.m_failWrite;
    return *this;
}

CSocketTest::CSocketTest(String const& addr_, SPtr<String> buf_, bool fail_)
    : m_buffer(buf_), m_address(addr_), m_failWrite(fail_) {}

auto
CSocketTest::Address() const -> String {
    return m_address;
}

auto
CSocketTest::Write(StringView const& msg_) -> bool {
    if (m_failWrite) {
        throw error::CSocketError("");
    }
    m_buffer->clear();
    m_buffer->append(msg_);
    return true;
}

void
CSocketTest::Close() {}
}  // namespace vfrb::server::net
