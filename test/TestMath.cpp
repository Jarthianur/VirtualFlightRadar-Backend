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

#include "math/Math.hpp"

#include "Helper.hpp"

using namespace vfrb;

SUITE_PAR("testMath") {
    TEST("Radian") {
        ASSERT(math::Radian(45.0), FEQ(), 0.785398);
        ASSERT(math::Radian(0.0), FEQ(), 0.0);
        ASSERT(math::Radian(360.0), FEQ(), 6.28319);
    }
    TEST("Degree") {
        ASSERT(math::Degree(0.785398), FEQ(), 45.0);
        ASSERT(math::Degree(0.0), FEQ(), 0.0);
        ASSERT(math::Degree(6.28319), FEQ(), 360.0);
    }
    TEST("DoubleToInt") {
        ASSERT_EQ(math::DoubleToInt(0.0), 0);
        ASSERT_EQ(math::DoubleToInt(1.4), 1);
        ASSERT_EQ(math::DoubleToInt(1.5), 2);
        ASSERT_EQ(math::DoubleToInt(-1.4), -1);
        ASSERT_EQ(math::DoubleToInt(-1.5), -2);
    }
    TEST("DmToDeg") {
        ASSERT(math::DmToDeg(0.0), FEQ(), 0.0);
        ASSERT(math::DmToDeg(9030.50), FEQ(), 90.508333);
        ASSERT(math::DmToDeg(18000.0), FEQ(), 180.0);
        ASSERT(math::DmToDeg(-4512.3456), FEQ(), 45.205760);
    }
    TEST("IcaoHeight") {
        ASSERT_EQ(math::IcaoHeight(0.0), 44331);
        ASSERT_EQ(math::IcaoHeight(1013.25), 0);
        ASSERT_EQ(math::IcaoHeight(980.0), 281);
    }
    TEST("Saturate") {
        ASSERT_EQ(math::Saturate(15, 0, 10), 10);
        ASSERT_EQ(math::Saturate(9, 0, 10), 9);
        ASSERT_EQ(math::Saturate(1, 3, 10), 3);
        ASSERT_EQ(math::Saturate(-5, -10, 10), -5);
        ASSERT_EQ(math::Saturate(-5, 0, 10), 0);
    };
};
