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

#include "../src/object/Aircraft.h"
#include "../src/object/Atmosphere.h"
#include "../src/object/GpsPosition.h"
#include "../src/object/TimeStamp.hpp"
#include "../src/object/Wind.h"
#include "DateTimeImplTest.h"
#include "helper.hpp"

using namespace object;
using namespace timestamp;
using namespace sctf;

using TS = TimeStamp<DateTimeImplTest>;

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
            Object o3(2);
            o1.set_serialized("");
            o2.set_serialized("a");
            o3.set_serialized("b");
            assertEquals(o1.get_serialized().size(), 0);
            assertTrue(o1.tryUpdate(std::move(o2)));
            assertEqStr(o1.get_serialized(), "a");
            for(int i = 0; i < OBJ_OUTDATED; ++i)
            {
                assertFalse(o3.tryUpdate(std::move(o1)));
                ++o3;
            }
            assertTrue(o3.tryUpdate(std::move(o1)));
        });

    describe<TS>("Basic TimeStamp tests", runner)
        ->setup([] {
            DateTimeImplTest::set_day(1);
            DateTimeImplTest::set_now(12, 0, 0);
        })
        ->test("creation - valid format",
               [] {
                   assertNoExcept(TS("120000", Format::HHMMSS));
                   assertNoExcept(TS("12:00:00.000", Format::HH_MM_SS_FFF));
               })
        ->test("creation - invalid format",
               [] {
                   assertException(TS("", Format::HHMMSS), std::invalid_argument);
                   assertException(TS("adgjhag", Format::HHMMSS), std::invalid_argument);
                   assertException(TS("36:60:11.11111", Format::HH_MM_SS_FFF),
                                   std::invalid_argument);
                   assertException(TS("366022", Format::HHMMSS), std::invalid_argument);
               })
        ->test("comparison - incremental time",
               [] {
                   DateTimeImplTest::set_day(1);
                   DateTimeImplTest::set_now(13, 0, 0);
                   TS t1("120000", Format::HHMMSS);
                   TS t2("120001", Format::HHMMSS);
                   assertTrue(t2 > t1);
                   assertFalse(t1 > t2);
               })
        ->test("comparison - old day msg",
               [] {
                   DateTimeImplTest::set_day(1);
                   DateTimeImplTest::set_now(23, 59, 0);
                   TS t1("235800", Format::HHMMSS);
                   DateTimeImplTest::set_day(2);
                   DateTimeImplTest::set_now(0, 0, 0);
                   TS t2("235900", Format::HHMMSS);
                   assertTrue(t2 > t1);
                   assertFalse(t1 > t2);
               })
        ->test("comparison - new day msg",
               [] {
                   DateTimeImplTest::set_day(1);
                   DateTimeImplTest::set_now(23, 59, 0);
                   TS t1("235800", Format::HHMMSS);
                   DateTimeImplTest::set_day(2);
                   DateTimeImplTest::set_now(0, 1, 0);
                   TS t2("000000", Format::HHMMSS);
                   assertTrue(t2 > t1);
                   assertFalse(t1 > t2);
               })
        ->test("comparison - incremental day", [] {
            DateTimeImplTest::set_day(1);
            DateTimeImplTest::set_now(13, 0, 0);
            TS t1("120000", Format::HHMMSS);
            DateTimeImplTest::set_day(2);
            TS t2("110000", Format::HHMMSS);
            assertTrue(t2 > t1);
            assertFalse(t1 > t2);
        });

    describe<GpsPosition>("Basic GpsPosition tests", runner)->test("assign", [] {
        GpsPosition pos1({2.0, 2.0, 2}, 41.0);
        GpsPosition pos2({1.0, 1.0, 1}, 48.0);
        pos2.set_timeStamp(
            TimeStamp<timestamp::DateTimeImplBoost>("120000", timestamp::Format::HHMMSS));
        assertTrue(pos1.tryUpdate(std::move(pos2)));
        assertEquals(pos1.get_geoid(), 48.0);
        assertEquals(pos1.get_position().latitude, 1.0);
    });
}
