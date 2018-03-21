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

#include <boost/regex.hpp>

#include "../../src/Math.hpp"
#include "../Helper.hpp"
#include "../framework/src/framework.h"

#ifdef assert
#undef assert
#endif

using namespace testsuite;
using namespace comparator;

void test_math(TestSuitesRunner& runner)
{
    describe("math utils", runner, "math")
        ->test("radian",
               []() {
                   assert(math::radian(45.0), 0.785398, helper::equalsD);
                   assert(math::radian(0.0), 0.0, helper::equalsD);
                   assert(math::radian(360.0), 6.28319, helper::equalsD);
               })
        ->test("degree",
               []() {
                   assert(math::degree(0.785398), 45.0, helper::equalsD);
                   assert(math::degree(0.0), 0.0, helper::equalsD);
                   assert(math::degree(6.28319), 360.0, helper::equalsD);
               })
        ->test("doubleToInt",
               []() {
                   assert(math::doubleToInt(0.0), 0, helper::equalsInt);
                   assert(math::doubleToInt(1.4), 1, helper::equalsInt);
                   assert(math::doubleToInt(1.5), 2, helper::equalsInt);
                   assert(math::doubleToInt(-1.4), -1, helper::equalsInt);
                   assert(math::doubleToInt(-1.5), -2, helper::equalsInt);
               })
        ->test("dmToDeg",
               []() {
                   assert(math::dmToDeg(0.0), 0.0, helper::equalsD);
                   assert(math::dmToDeg(9030.50), 90.508333, helper::equalsD);
                   assert(math::dmToDeg(18000.0), 180.0, helper::equalsD);
                   assert(math::dmToDeg(-4512.3456), 45.205760, helper::equalsD);
               })
        ->test("calcIcaoHeight",
               []() {
                   assert(math::icaoHeight(0.0), 44331, helper::equalsInt);
                   assert(math::icaoHeight(1013.25), 0, helper::equalsInt);
                   assert(math::icaoHeight(980.0), 281, helper::equalsInt);
               })
        ->test("checksum", []() {
            assert(math::checksum("", sizeof("")), 0, helper::equalsInt);
            assert(math::checksum("\0", sizeof("\0")), 0, helper::equalsInt);
            assert(math::checksum("$abc*", sizeof("$abc*")), 96, helper::equalsInt);
        });
}
