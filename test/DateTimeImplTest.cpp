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

#include "types.h"

namespace vfrb::object::date_time
{
namespace
{
static thread_local u32 s_Day = 0;
static thread_local s64 s_Now = 0;
}  // namespace

void Day(u32 d_)
{
    s_Day = d_;
}

void Now(u32 h_, u32 m_, u32 s_)
{
    s_Now = h_ * 3600000 + m_ * 60000 + s_ * 1000;
}

s64 Now()
{
    return s_Now;
}

u32 Day()
{
    return s_Day;
}
}  // namespace vfrb::object::date_time
