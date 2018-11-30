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

#include "DateTimeImplTest.h"

std::int64_t DateTimeImplTest::_now  = 0;
std::uint32_t DateTimeImplTest::_day = 0;

std::int64_t DateTimeImplTest::now()
{
    return _now;
}
std::uint32_t DateTimeImplTest::day()
{
    return _day;
}
void DateTimeImplTest::set_day(std::uint32_t d)
{
    _day = d;
}
void DateTimeImplTest::set_now(std::uint32_t h, std::uint32_t m, std::uint32_t s)
{
    _now = h * 3600000 + m * 60000 + s * 1000;
}
