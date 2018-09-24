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

#include "../../src/object/Aircraft.h"
#include "../../src/object/Atmosphere.h"
#include "../../src/object/GpsPosition.h"
#include "../../src/object/TimeStamp.hpp"
#include "../../src/object/Wind.h"
#include "../Helper.hpp"

using namespace object;
using namespace sctf;

void test_object(test::TestSuitesRunner& runner)
{
    describe<Object>("Basic Object tests", runner)
        ->test("aging",
               [] {
                   Object o;
                   assertEquals(o.get_updateAge(), 0);
                   assertEquals((++o).get_updateAge(), 1);
               })
        ->test("tryUpdate", [] {
            Object o1;
            Object o2(1);
            o1.set_serialized("");
            o2.set_serialized("a");
            assertEquals(o1.get_serialized().size(), 0);
            assertTrue(o1.tryUpdate(std::move(o2)));
            assertEqStr(o1.get_serialized(), "a");
            for(int i = 0; i < OBJ_OUTDATED; ++i)
            {
                assertFalse(o2.tryUpdate(std::move(o1)));
                ++o2;
            }
            assertTrue(o2.tryUpdate(std::move(o1)));
        });

    describe<TimeStamp>("Basic TimeStamp tests", runner)
        ->test("creation - valid format",
               [] {
                   assertNoExcept(TimeStamp("120000", TimeStamp::Format::HHMMSS));
                   assertNoExcept(TimeStamp("12:00:00.000", TimeStamp::Format::HH_MM_SS_FFF));
               })
        ->test("creation - invalid format",
               [] {
                   assertException(TimeStamp("", TimeStamp::Format::HHMMSS), std::invalid_argument);
                   assertException(TimeStamp("adgjhag", TimeStamp::Format::HHMMSS),
                                   std::invalid_argument);
                   assertException(TimeStamp("36:60:11.11111", TimeStamp::Format::HH_MM_SS_FFF),
                                   std::invalid_argument);
                   assertException(TimeStamp("366022", TimeStamp::Format::HHMMSS),
                                   std::invalid_argument);
               })
        ->test("comparison - incremental time",
               [] {
                   TimeStamp t1("120000", TimeStamp::Format::HHMMSS);
                   TimeStamp t2("120001", TimeStamp::Format::HHMMSS);
                   assertTrue(t2 > t1);
                   assertFalse(t1 > t2);
               })
        ->test("comparison - day change", [] {
            TimeStamp t1("235959", TimeStamp::Format::HHMMSS);
            TimeStamp t2("000000", TimeStamp::Format::HHMMSS);
            assertTrue(t2 > t1);
            assertFalse(t1 > t2);
        });
}
