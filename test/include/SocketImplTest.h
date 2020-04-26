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

#include "util/class_utils.h"

#include "types.h"

namespace vfrb
{
namespace server
{
namespace net
{
class CSocketImplTest
{
    SPtr<Str> m_buffer;
    Str       m_address;
    bool      m_failWrite = false;

public:
    MOVABLE_BUT_NOT_COPYABLE(CSocketImplTest)

    CSocketImplTest(Str const& addr_, SPtr<Str> buf_, bool fail_);
    ~CSocketImplTest() noexcept = default;

    Str  Address() const;
    bool Write(StrView const& msg_);
    void Close();
};
}  // namespace net
}  // namespace server
}  // namespace vfrb
