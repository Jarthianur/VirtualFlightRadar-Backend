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

#include "../src/util/math.hpp"
#include "helper.hpp"

using namespace sctf;

void test_math(test::TestSuitesRunner& runner)
{
    describe("math utils", runner, "math")
        ->test("radian",
               []() {
                   assertEquals(math::radian(45.0), 0.785398);
                   assertEquals(math::radian(0.0), 0.0);
                   assertEquals(math::radian(360.0), 6.28319);
               })
        ->test("degree",
               []() {
                   assertEquals(math::degree(0.785398), 45.0);
                   assertEquals(math::degree(0.0), 0.0);
                   assertEquals(math::degree(6.28319), 360.0);
               })
        ->test("doubleToInt",
               []() {
                   assertEquals(math::doubleToInt(0.0), 0);
                   assertEquals(math::doubleToInt(1.4), 1);
                   assertEquals(math::doubleToInt(1.5), 2);
                   assertEquals(math::doubleToInt(-1.4), -1);
                   assertEquals(math::doubleToInt(-1.5), -2);
               })
        ->test("dmToDeg",
               []() {
                   assertEquals(math::dmToDeg(0.0), 0.0);
                   assertEquals(math::dmToDeg(9030.50), 90.508333);
                   assertEquals(math::dmToDeg(18000.0), 180.0);
                   assertEquals(math::dmToDeg(-4512.3456), 45.205760);
               })
        ->test("calcIcaoHeight",
               []() {
                   assertEquals(math::icaoHeight(0.0), 44331);
                   assertEquals(math::icaoHeight(1013.25), 0);
                   assertEquals(math::icaoHeight(980.0), 281);
               })
        ->test("checksum", []() {
            assertEquals(math::checksum("", sizeof("")), 0);
            assertEquals(math::checksum("\0", sizeof("\0")), 0);
            assertEquals(math::checksum("$abc*", sizeof("$abc*")), 96);
        });
}
