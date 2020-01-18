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

#include "math/math.hpp"

#include "helper.hpp"

using namespace sctf;
using namespace vfrb;

TEST_MODULE(test_math, {
    test("radian", [] {
        ASSERT_EQUALS(math::Radian(45.0), 0.785398);
        ASSERT_EQUALS(math::Radian(0.0), 0.0);
        ASSERT_EQUALS(math::Radian(360.0), 6.28319);
    });
    test("degree", [] {
        ASSERT_EQUALS(math::Degree(0.785398), 45.0);
        ASSERT_EQUALS(math::Degree(0.0), 0.0);
        ASSERT_EQUALS(math::Degree(6.28319), 360.0);
    });
    test("doubleToInt", [] {
        ASSERT_EQUALS(math::DoubleToInt(0.0), 0);
        ASSERT_EQUALS(math::DoubleToInt(1.4), 1);
        ASSERT_EQUALS(math::DoubleToInt(1.5), 2);
        ASSERT_EQUALS(math::DoubleToInt(-1.4), -1);
        ASSERT_EQUALS(math::DoubleToInt(-1.5), -2);
    });
    test("dmToDeg", [] {
        ASSERT_EQUALS(math::DmToDeg(0.0), 0.0);
        ASSERT_EQUALS(math::DmToDeg(9030.50), 90.508333);
        ASSERT_EQUALS(math::DmToDeg(18000.0), 180.0);
        ASSERT_EQUALS(math::DmToDeg(-4512.3456), 45.205760);
    });
    test("calcIcaoHeight", [] {
        ASSERT_EQUALS(math::IcaoHeight(0.0), 44331);
        ASSERT_EQUALS(math::IcaoHeight(1013.25), 0);
        ASSERT_EQUALS(math::IcaoHeight(980.0), 281);
    });
    test("saturate", [] {
        ASSERT_EQUALS(math::Saturate(15, 0, 10), 10);
        ASSERT_EQUALS(math::Saturate(9, 0, 10), 9);
        ASSERT_EQUALS(math::Saturate(1, 3, 10), 3);
        ASSERT_EQUALS(math::Saturate(-5, -10, 10), -5);
        ASSERT_EQUALS(math::Saturate(-5, 0, 10), 0);
    });
})
