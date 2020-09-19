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

#include "Types.hpp"

namespace vfrb::object::date_time
{
namespace
{
thread_local u32 s_Day = 0;
thread_local s64 s_Now = 0;
}  // namespace

void
Day(u32 d_) {
    s_Day = d_;
}

void
Now(u32 h_, u32 m_, u32 s_) {
    s_Now = h_ * 3600000 + m_ * 60000 + s_ * 1000;
}

auto
Now() -> s64 {
    return s_Now;
}

auto
Day() -> u32 {
    return s_Day;
}
}  // namespace vfrb::object::date_time
