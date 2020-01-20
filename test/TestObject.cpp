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
using namespace std::literals;

namespace vfrb::object::date_time
{
extern void Now(u32, u32, u32);
extern void Day(s64);
}  // namespace vfrb::object::date_time

TEST_MODULE(test_Wind, {
    test("aging", [] {
        CWind w;
        ASSERT_EQUALS(w.UpdateAge(), 0);
        ++w;
        ASSERT_EQUALS(w.UpdateAge(), 1);
    });
    test("TryUpdate - higher priority", [] {
        CWind w(0, "1");
        CWind w2(1, "2");
        ++w;
        ASSERT_EQUALS(w.UpdateAge(), 1);
        ASSERT_TRUE(w.TryUpdate(std::move(w2)));
        ASSERT_EQUALS(w.UpdateAge(), 0);
        ASSERT_EQUALS(w.Nmea(), "2"s);
    });
    test("TryUpdate - equal priority", [] {
        CWind w(0, "1");
        CWind w2(0, "2");
        ++w;
        ASSERT_EQUALS(w.UpdateAge(), 1);
        ASSERT_TRUE(w.TryUpdate(std::move(w2)));
        ASSERT_EQUALS(w.UpdateAge(), 0);
        ASSERT_EQUALS(w.Nmea(), "2"s);
    });
    test("TryUpdate - outdated", [] {
        CWind w(1, "1");
        CWind w2(0, "2");
        while ((++w).UpdateAge() < CObject::OUTDATED)
            ;
        ASSERT_EQUALS(w.UpdateAge(), CObject::OUTDATED);
        ASSERT_TRUE(w.TryUpdate(std::move(w2)));
        ASSERT_EQUALS(w.UpdateAge(), 0);
        ASSERT_EQUALS(w.Nmea(), "2"s);
    });
    test("TryUpdate - lower priority", [] {
        CWind w(1, "1");
        CWind w2(0, "2");
        ++w;
        ASSERT_EQUALS(w.UpdateAge(), 1);
        ASSERT_FALSE(w.TryUpdate(std::move(w2)));
        ASSERT_EQUALS(w.UpdateAge(), 1);
        ASSERT_EQUALS(w.Nmea(), "1"s);
    });
})

TEST_MODULE(test_Atmosphere, {
    test("aging", [] {
        CAtmosphere a;
        ASSERT_EQUALS(a.UpdateAge(), 0);
        ++a;
        ASSERT_EQUALS(a.UpdateAge(), 1);
    });
    test("TryUpdate - higher priority", [] {
        CAtmosphere a(0, "1");
        CAtmosphere a2(1, 1.0, "2");
        ++a;
        ASSERT_EQUALS(a.UpdateAge(), 1);
        ASSERT_TRUE(a.TryUpdate(std::move(a2)));
        ASSERT_EQUALS(a.UpdateAge(), 0);
        ASSERT_EQUALS(a.Nmea(), "2"s);
        ASSERT_EQUALS(a.Pressure(), 1.0);
    });
    test("TryUpdate - equal priority", [] {
        CAtmosphere a(0, "1");
        CAtmosphere a2(0, 1.0, "2");
        ++a;
        ASSERT_EQUALS(a.UpdateAge(), 1);
        ASSERT_TRUE(a.TryUpdate(std::move(a2)));
        ASSERT_EQUALS(a.UpdateAge(), 0);
        ASSERT_EQUALS(a.Nmea(), "2"s);
        ASSERT_EQUALS(a.Pressure(), 1.0);
    });
    test("TryUpdate - outdated", [] {
        CAtmosphere a(1, "1");
        CAtmosphere a2(0, 1.0, "2");
        while ((++a).UpdateAge() < CObject::OUTDATED)
            ;
        ASSERT_EQUALS(a.UpdateAge(), CObject::OUTDATED);
        ASSERT_TRUE(a.TryUpdate(std::move(a2)));
        ASSERT_EQUALS(a.UpdateAge(), 0);
        ASSERT_EQUALS(a.Nmea(), "2"s);
        ASSERT_EQUALS(a.Pressure(), 1.0);
    });
    test("TryUpdate - lower priority", [] {
        CAtmosphere a(1, 2.0, "1");
        CAtmosphere a2(0, 1.0, "2");
        ++a;
        ASSERT_EQUALS(a.UpdateAge(), 1);
        ASSERT_FALSE(a.TryUpdate(std::move(a2)));
        ASSERT_EQUALS(a.UpdateAge(), 1);
        ASSERT_EQUALS(a.Nmea(), "1"s);
        ASSERT_EQUALS(a.Pressure(), 2.0);
    });
    test("init - fail", [] {
        ASSERT_THROWS(CAtmosphere(0, -1.0, ""), util::error::CLimitsExceededError);
        ASSERT_THROWS(CAtmosphere(0, 2001.0, ""), util::error::CLimitsExceededError);
    });
})

TEST_MODULE(test_GpsPosition, {
    setup([] {
        date_time::Day(1);
        date_time::Now(12, 0, 0);
    });
    test("aging", [] {
        CGpsPosition p(0, {1., 1., 1}, 1.);
        ASSERT_EQUALS(p.UpdateAge(), 0);
        ++p;
        ASSERT_EQUALS(p.UpdateAge(), 1);
    });
    test("TryUpdate - higher priority", [] {
        CGpsPosition p(0, {1., 1., 1}, 1.);
        CGpsPosition p2(1, {2., 2., 2}, 2., 2., 4, 6, CTimestamp("115900"));
        ++p;
        ASSERT_EQUALS(p.UpdateAge(), 1);
        ASSERT_TRUE(p.TryUpdate(std::move(p2)));
        ASSERT_EQUALS(p.UpdateAge(), 0);
        ASSERT_EQUALS(p.Geoid(), 2.);
        ASSERT_EQUALS(p.Dilution(), 2.);
        ASSERT_EQUALS(p.FixQuality(), 6);
        ASSERT_EQUALS(p.NrOfSatellites(), 4);
        ASSERT_EQUALS(p.Location(), (SLocation{2., 2., 2}));
    });
    test("TryUpdate - equal priority", [] {
        CGpsPosition p(0, {1., 1., 1}, 1.);
        CGpsPosition p2(0, {2., 2., 2}, 2., 2., 4, 6, CTimestamp("115900"));
        ++p;
        ASSERT_EQUALS(p.UpdateAge(), 1);
        ASSERT_TRUE(p.TryUpdate(std::move(p2)));
        ASSERT_EQUALS(p.UpdateAge(), 0);
        ASSERT_EQUALS(p.Geoid(), 2.);
        ASSERT_EQUALS(p.Dilution(), 2.);
        ASSERT_EQUALS(p.FixQuality(), 6);
        ASSERT_EQUALS(p.NrOfSatellites(), 4);
        ASSERT_EQUALS(p.Location(), (SLocation{2., 2., 2}));
    });
    test("TryUpdate - outdated", [] {
        CGpsPosition p(1, {1., 1., 1}, 1.);
        CGpsPosition p2(0, {2., 2., 2}, 2., 2., 4, 6, CTimestamp("115900"));
        while ((++p).UpdateAge() < CObject::OUTDATED)
            ;
        ASSERT_EQUALS(p.UpdateAge(), CObject::OUTDATED);
        ASSERT_TRUE(p.TryUpdate(std::move(p2)));
        ASSERT_EQUALS(p.UpdateAge(), 0);
        ASSERT_EQUALS(p.Geoid(), 2.);
        ASSERT_EQUALS(p.Dilution(), 2.);
        ASSERT_EQUALS(p.FixQuality(), 6);
        ASSERT_EQUALS(p.NrOfSatellites(), 4);
        ASSERT_EQUALS(p.Location(), (SLocation{2., 2., 2}));
    });
    test("TryUpdate - lower priority", [] {
        CGpsPosition p(1, {1., 1., 1}, 1., 1., 3, 5, CTimestamp("115800"));
        CGpsPosition p2(0, {2., 2., 2}, 2., 2., 4, 6, CTimestamp("115900"));
        ++p;
        ASSERT_EQUALS(p.UpdateAge(), 1);
        ASSERT_FALSE(p.TryUpdate(std::move(p2)));
        ASSERT_EQUALS(p.UpdateAge(), 1);
        ASSERT_EQUALS(p.Geoid(), 1.);
        ASSERT_EQUALS(p.Dilution(), 1.);
        ASSERT_EQUALS(p.FixQuality(), 5);
        ASSERT_EQUALS(p.NrOfSatellites(), 3);
        ASSERT_EQUALS(p.Location(), (SLocation{1., 1., 1}));
    });
    test("TryUpdate - older timestamp", [] {
        CGpsPosition p(0, {1., 1., 1}, 1., 1., 3, 5, CTimestamp("115900"));
        CGpsPosition p2(1, {2., 2., 2}, 2., 2., 4, 6, CTimestamp("115800"));
        ++p;
        ASSERT_EQUALS(p.UpdateAge(), 1);
        ASSERT_FALSE(p.TryUpdate(std::move(p2)));
        ASSERT_EQUALS(p.UpdateAge(), 1);
        ASSERT_EQUALS(p.Geoid(), 1.);
        ASSERT_EQUALS(p.Dilution(), 1.);
        ASSERT_EQUALS(p.FixQuality(), 5);
        ASSERT_EQUALS(p.NrOfSatellites(), 3);
        ASSERT_EQUALS(p.Location(), (SLocation{1., 1., 1}));
    });
    test("init - fail", [] {
        ASSERT_THROWS(CGpsPosition p(0, {SLocation::MIN_LATITUDE - 1., 1., 1}, 1.);
                      , util::error::CLimitsExceededError);
        ASSERT_THROWS(CGpsPosition p(0, {SLocation::MAX_LATITUDE + 1., 1., 1}, 1.);
                      , util::error::CLimitsExceededError);
        ASSERT_THROWS(CGpsPosition p(0, {1., SLocation::MIN_LONGITUDE - 1., 1}, 1.);
                      , util::error::CLimitsExceededError);
        ASSERT_THROWS(CGpsPosition p(0, {1., SLocation::MAX_LONGITUDE + 1., 1}, 1.);
                      , util::error::CLimitsExceededError);
        ASSERT_THROWS(CGpsPosition p(0, {1., 1., SLocation::MIN_ALTITUDE - 1}, 1.);
                      , util::error::CLimitsExceededError);
        ASSERT_THROWS(CGpsPosition p(0, {1., 1., SLocation::MAX_ALTITUDE + 1}, 1.);
                      , util::error::CLimitsExceededError);
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
