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

#include "object/Aircraft.h"
#include "object/Atmosphere.h"
#include "object/GpsPosition.h"
#include "object/TimeStamp.hpp"
#include "object/Wind.h"

#include "DateTimeImplTest.h"
#include "helper.hpp"

using namespace object;
using namespace time;
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
            for (int i = 0; i < OBJ_OUTDATED; ++i)
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

    describe<GpsPosition>("Basic GpsPosition tests", runner)
        ->test("update - successful",
               [] {
                   GpsPosition pos1({2.0, 2.0, 2}, 41.0);
                   GpsPosition pos2({1.0, 1.0, 1}, 48.0);
                   pos2.set_timeStamp(TimeStamp<time::DateTimeImplBoost>(
                       "120000", time::Format::HHMMSS));
                   assertTrue(pos1.tryUpdate(std::move(pos2)));
                   assertEquals(pos1.get_geoid(), 48.0);
                   assertEquals(pos1.get_position().latitude, 1.0);
               })
        ->test("update - failing", [] {
            GpsPosition pos1({2.0, 2.0, 2}, 41.0);
            GpsPosition pos2({1.0, 1.0, 1}, 48.0);
            pos1.set_timeStamp(
                TimeStamp<time::DateTimeImplBoost>("120100", time::Format::HHMMSS));
            pos2.set_timeStamp(
                TimeStamp<time::DateTimeImplBoost>("120000", time::Format::HHMMSS));
            assertFalse(pos1.tryUpdate(std::move(pos2)));
            assertEquals(pos1.get_geoid(), 41.0);
            assertEquals(pos1.get_position().latitude, 2.0);
        });

    describe<Atmosphere>("Basic Atmosphere tests", runner)
        ->test("update - successful",
               [] {
                   Atmosphere a1(1.0, 0);
                   Atmosphere a2(2.0, 1);
                   assertTrue(a1.tryUpdate(std::move(a2)));
                   assertEquals(a1.get_pressure(), 2.0);
               })
        ->test("update - failing", [] {
            Atmosphere a1(1.0, 0);
            Atmosphere a2(2.0, 1);
            assertFalse(a2.tryUpdate(std::move(a1)));
            assertEquals(a2.get_pressure(), 2.0);
        });

    describe<Aircraft>("Basic Aircraft tests", runner)
        ->test("update same target type",
               [] {
                   Aircraft a1;
                   Aircraft a2(1);
                   a1.set_serialized("a1");
                   a2.set_serialized("a2");
                   a2.set_timeStamp(
                       TimeStamp<DateTimeImplBoost>("120000", time::Format::HHMMSS));
                   assertTrue(a1.tryUpdate(std::move(a2)));
                   assertEqStr(a1.get_serialized(), "a2");
                   Aircraft a3;
                   a1.set_targetType(Aircraft::TargetType::FLARM);
                   a3.set_targetType(Aircraft::TargetType::FLARM);
                   assertTrue(a3.tryUpdate(std::move(a1)));
               })
        ->test("update different target type", [] {
            Aircraft a1;
            Aircraft a2;
            a2.set_timeStamp(TimeStamp<DateTimeImplBoost>("120000", time::Format::HHMMSS));
            a2.set_targetType(Aircraft::TargetType::FLARM);
            assertTrue(a1.tryUpdate(std::move(a2)));
            assertFalse(a2.tryUpdate(std::move(a1)));
            do
            {
                ++a2;
            } while (a2.get_updateAge() <= OBJ_OUTDATED);
            a1.set_timeStamp(TimeStamp<DateTimeImplBoost>("120100", time::Format::HHMMSS));
            assertTrue(a2.tryUpdate(std::move(a1)));
        });
}
