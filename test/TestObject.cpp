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
#include "object/Timestamp.h"
#include "object/Wind.h"

#include "helper.hpp"

using namespace vfrb;
using namespace object;
using namespace sctf;

namespace vfrb::object::date_time
{
extern void Now(u32, u32, u32);
extern void Day(s64);
}  // namespace vfrb::object::date_time

TEST_MODULE(test_object, {
    test("aging", [] {
        CObject o(0);
        assertEquals(o.get_updateAge(), 0);
        assertEquals((++o).get_updateAge(), 1);
    });
    test("tryUpdate", [] {
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
})

TEST_MODULE(test_gps_position, {
    test("update - successful", [] {
        GpsPosition pos1({2.0, 2.0, 2}, 41.0, 0);
        GpsPosition pos2({1.0, 1.0, 1}, 48.0, 0,
                         Timestamp<time::DateTimeImplBoost>("120000", time::Format::HHMMSS));
        assertTrue(pos1.tryUpdate(std::move(pos2)));
        assertEquals(pos1.get_geoid(), 48.0);
        assertEquals(pos1.get_position().latitude, 1.0);
    });
    test("update - failing", [] {
        GpsPosition pos1({2.0, 2.0, 2}, 41.0);
        GpsPosition pos2({1.0, 1.0, 1}, 48.0);
        pos1.set_timeStamp(Timestamp<time::DateTimeImplBoost>("120100", time::Format::HHMMSS));
        pos2.set_timeStamp(Timestamp<time::DateTimeImplBoost>("120000", time::Format::HHMMSS));
        assertFalse(pos1.tryUpdate(std::move(pos2)));
        assertEquals(pos1.get_geoid(), 41.0);
        assertEquals(pos1.get_position().latitude, 2.0);
    });
})

TEST_MODULE(test_atmosphere, {
    test("update - successful", [] {
        Atmosphere a1(1.0, 0);
        Atmosphere a2(2.0, 1);
        assertTrue(a1.tryUpdate(std::move(a2)));
        assertEquals(a1.get_pressure(), 2.0);
    });
    test("update - failing", [] {
        Atmosphere a1(1.0, 0);
        Atmosphere a2(2.0, 1);
        assertFalse(a2.tryUpdate(std::move(a1)));
        assertEquals(a2.get_pressure(), 2.0);
    });
})

TEST_MODULE(test_aircraft, {
    test("update same target type", [] {
        Aircraft a1(0, "123456", Aircraft::IdType::RANDOM, Aircraft::AircraftType::UNKNOWN, {.0, .0, 0},
                    {.0, .0, .0}, Timestamp<DateTimeImplBoost>("120000", time::Format::HHMMSS));
        Aircraft a2(0, "123456", Aircraft::IdType::ICAO, Aircraft::AircraftType::UNKNOWN, {.0, .0, 0},
                    {.0, .0, .0}, Timestamp<DateTimeImplBoost>("130000", time::Format::HHMMSS));
        assertTrue(a1.tryUpdate(std::move(a2)));
        assertEquals(a1.getIdType(), Aircraft::IdType::ICAO);
        Aircraft a3;
        a1.set_targetType(Aircraft::TargetType::FLARM);
        a3.set_targetType(Aircraft::TargetType::FLARM);
        assertTrue(a3.tryUpdate(std::move(a1)));
    });
    test("update different target type", [] {
        Aircraft a1;
        Aircraft a2;
        a2.set_timeStamp(Timestamp<DateTimeImplBoost>("120000", time::Format::HHMMSS));
        a2.set_targetType(Aircraft::TargetType::FLARM);
        assertTrue(a1.tryUpdate(std::move(a2)));
        assertFalse(a2.tryUpdate(std::move(a1)));
        do
        {
            ++a2;
        } while (a2.get_updateAge() <= OBJ_OUTDATED);
        a1.set_timeStamp(Timestamp<DateTimeImplBoost>("120100", time::Format::HHMMSS));
        assertTrue(a2.tryUpdate(std::move(a1)));
    });
})

TEST_MODULE(test_wind, {
    test("aging", [] {
        CWind w;
        ASSERT_EQUALS(w.UpdateAge(), 0);
        ++w;
        ASSERT_EQUALS(w.UpdateAge(), 1);
    });
    test("TryUpdate - higher priority", [] {
        CWind w(0);
        CWind w2(1);
        ++w;
        ASSERT_EQUALS(w.UpdateAge(), 1);
        ASSERT_TRUE(w.TryUpdate(std::move(w2)));
        ASSERT_EQUALS(w.UpdateAge(), 0);
    });
    test("TryUpdate - outdated", [] {
        CWind w(1);
        CWind w2(0);
        while (w.UpdateAge() < CObject::OUTDATED)
        {
            ++w;
        }
        ASSERT_EQUALS(w.UpdateAge(), CObject::OUTDATED);
        ASSERT_TRUE(w.TryUpdate(std::move(w2)));
        ASSERT_EQUALS(w.UpdateAge(), 0);
    });
})

void test_timestamp()
{
    suite<CTimestamp>("Basic Timestamp tests")
        ->setup([] {
            date_time::Day(1);
            date_time::Now(12, 0, 0);
        })
        ->test("creation - valid format",
               [] {
                   ASSERT_NOTHROW(CTimestamp("120000"));
                   ASSERT_NOTHROW(CTimestamp("12:00:00.000"));
               })
        ->test("creation - invalid format",
               [] {
                   ASSERT_THROWS(CTimestamp(""), vfrb::object::error::CTimestampParseError);
                   ASSERT_THROWS(CTimestamp("adgjhag"), vfrb::object::error::CTimestampParseError);
                   ASSERT_THROWS(CTimestamp("36:60:11.11111"), vfrb::object::error::CTimestampParseError);
                   ASSERT_THROWS(CTimestamp("366022"), vfrb::object::error::CTimestampParseError);
               })
        ->test("comparison - incremental time",
               [] {
                   date_time::Day(1);
                   date_time::Now(13, 0, 0);
                   CTimestamp const t1("120000");
                   CTimestamp const t2("120001");

                   ASSERT(t2, GT, t1);
                   ASSERT(t1, !GT, t2);
               })
        ->test("comparison - old day msg",
               [] {
                   date_time::Day(1);
                   date_time::Now(23, 59, 0);
                   CTimestamp const t1("235800");
                   date_time::Day(2);
                   date_time::Now(0, 0, 0);
                   CTimestamp const t2("235900");

                   ASSERT(t2, GT, t1);
                   ASSERT(t1, !GT, t2);
               })
        ->test("comparison - new day msg",
               [] {
                   date_time::Day(1);
                   date_time::Now(23, 59, 0);
                   CTimestamp const t1("235800");
                   date_time::Day(2);
                   date_time::Now(0, 1, 0);
                   CTimestamp const t2("000000");

                   ASSERT(t2, GT, t1);
                   ASSERT(t1, !GT, t2);
               })
        ->test("comparison - incremental day", [] {
            date_time::Day(1);
            date_time::Now(13, 0, 0);
            CTimestamp const t1("120000");
            date_time::Day(2);
            CTimestamp const t2("110000");

            ASSERT(t2, GT, t1);
            ASSERT(t1, !GT, t2);
        });
}
