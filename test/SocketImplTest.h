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

#pragma once

#include <cstdint>
#include <string>

#include "util/defines.h"

namespace server
{
class SocketImplTest
{
public:
    MOVABLE_BUT_NOT_COPYABLE(SocketImplTest)

    explicit SocketImplTest(int&& socket);
    ~SocketImplTest() noexcept;

    std::string get_address() const;
    bool write(const std::string& msg);
    void close();
    int& get();

private:
    std::string m_buffer;
    std::int32_t m_socket;
    std::string m_address;

public:
    GETTER_CR(buffer)
    SETTER_CR(address)
};

}  // namespace server
