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

TEST_MODULE_PAR(test_Wind, {
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

TEST_MODULE_PAR(test_Atmosphere, {
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

TEST_MODULE_PAR(test_GpsPosition, {
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

TEST_MODULE_PAR(test_aircraft, {
    setup([] {
        date_time::Day(1);
        date_time::Now(12, 0, 0);
    });
    test("aging", [] {
        CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN, {1., 1., 1},
                    CTimestamp("115900"));
        ASSERT_EQUALS(a.UpdateAge(), 0);
        ++a;
        ASSERT_EQUALS(a.UpdateAge(), 1);
    });
    test("TryUpdate - higher priority", [] {
        {
            CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        {1., 1., 1}, CTimestamp("115800"));
            CAircraft a2(1, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                         {1., 1., 1.}, CTimestamp("115900"));
            a2.TargetType(CAircraft::ETargetType::TRANSPONDER);
            ++a;
            ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQUALS(a.UpdateAge(), 1);
            ASSERT_TRUE(a.TryUpdate(std::move(a2)));
            ASSERT_EQUALS(a.UpdateAge(), 0);
            ASSERT_EQUALS(a.IdType(), CAircraft::EIdType::ICAO);
            ASSERT_EQUALS(a.AircraftType(), CAircraft::EAircraftType::UFO);
            ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQUALS(a.Location(), (SLocation{2., 2., 2}));
            ASSERT_EQUALS(a.Movement(), (CAircraft::SMovement{1., 1., 1.}));
            ASSERT_EQUALS(a.Timestamp(), CTimestamp("115900"));
            ASSERT_TRUE(a.HasFullInfo());
        }
        {
            CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        {1., 1., 1}, CTimestamp("115800"));
            CAircraft a2(1, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                         {1., 1., 1.}, CTimestamp("115900"));
            a.TargetType(CAircraft::ETargetType::FLARM);
            ++a;
            ASSERT_EQUALS(a2.TargetType(), CAircraft::ETargetType::FLARM);
            ASSERT_EQUALS(a.UpdateAge(), 1);
            ASSERT_TRUE(a.TryUpdate(std::move(a2)));
            ASSERT_EQUALS(a.UpdateAge(), 0);
            ASSERT_EQUALS(a.IdType(), CAircraft::EIdType::ICAO);
            ASSERT_EQUALS(a.AircraftType(), CAircraft::EAircraftType::UFO);
            ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQUALS(a.Location(), (SLocation{2., 2., 2}));
            ASSERT_EQUALS(a.Movement(), (CAircraft::SMovement{1., 1., 1.}));
            ASSERT_EQUALS(a.Timestamp(), CTimestamp("115900"));
            ASSERT_TRUE(a.HasFullInfo());
        }
    });
    test("TryUpdate - equal priority", [] {
        {
            CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        {1., 1., 1}, CTimestamp("115800"));
            CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                         {1., 1., 1.}, CTimestamp("115900"));
            a2.TargetType(CAircraft::ETargetType::TRANSPONDER);
            ++a;
            ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQUALS(a.UpdateAge(), 1);
            ASSERT_TRUE(a.TryUpdate(std::move(a2)));
            ASSERT_EQUALS(a.UpdateAge(), 0);
            ASSERT_EQUALS(a.IdType(), CAircraft::EIdType::ICAO);
            ASSERT_EQUALS(a.AircraftType(), CAircraft::EAircraftType::UFO);
            ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQUALS(a.Location(), (SLocation{2., 2., 2}));
            ASSERT_EQUALS(a.Movement(), (CAircraft::SMovement{1., 1., 1.}));
            ASSERT_EQUALS(a.Timestamp(), CTimestamp("115900"));
            ASSERT_TRUE(a.HasFullInfo());
        }
        {
            CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        {1., 1., 1}, CTimestamp("115800"));
            CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                         {1., 1., 1.}, CTimestamp("115900"));
            a.TargetType(CAircraft::ETargetType::FLARM);
            ++a;
            ASSERT_EQUALS(a2.TargetType(), CAircraft::ETargetType::FLARM);
            ASSERT_EQUALS(a.UpdateAge(), 1);
            ASSERT_TRUE(a.TryUpdate(std::move(a2)));
            ASSERT_EQUALS(a.UpdateAge(), 0);
            ASSERT_EQUALS(a.IdType(), CAircraft::EIdType::ICAO);
            ASSERT_EQUALS(a.AircraftType(), CAircraft::EAircraftType::UFO);
            ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQUALS(a.Location(), (SLocation{2., 2., 2}));
            ASSERT_EQUALS(a.Movement(), (CAircraft::SMovement{1., 1., 1.}));
            ASSERT_EQUALS(a.Timestamp(), CTimestamp("115900"));
            ASSERT_TRUE(a.HasFullInfo());
        }
    });
    test("TryUpdate - outdated", [] {
        {
            CAircraft a(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        {1., 1., 1}, CTimestamp("115800"));
            CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                         {1., 1., 1.}, CTimestamp("115900"));
            a2.TargetType(CAircraft::ETargetType::TRANSPONDER);
            while ((++a).UpdateAge() < CObject::OUTDATED)
                ;
            ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQUALS(a.UpdateAge(), CObject::OUTDATED);
            ASSERT_TRUE(a.TryUpdate(std::move(a2)));
            ASSERT_EQUALS(a.UpdateAge(), 0);
            ASSERT_EQUALS(a.IdType(), CAircraft::EIdType::ICAO);
            ASSERT_EQUALS(a.AircraftType(), CAircraft::EAircraftType::UFO);
            ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQUALS(a.Location(), (SLocation{2., 2., 2}));
            ASSERT_EQUALS(a.Movement(), (CAircraft::SMovement{1., 1., 1.}));
            ASSERT_EQUALS(a.Timestamp(), CTimestamp("115900"));
            ASSERT_TRUE(a.HasFullInfo());
        }
        {
            CAircraft a(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        {1., 1., 1}, CTimestamp("115800"));
            CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                         {1., 1., 1.}, CTimestamp("115900"));
            a.TargetType(CAircraft::ETargetType::FLARM);
            while ((++a).UpdateAge() < CObject::OUTDATED)
                ;
            ASSERT_EQUALS(a2.TargetType(), CAircraft::ETargetType::FLARM);
            ASSERT_EQUALS(a.UpdateAge(), CObject::OUTDATED);
            ASSERT_TRUE(a.TryUpdate(std::move(a2)));
            ASSERT_EQUALS(a.UpdateAge(), 0);
            ASSERT_EQUALS(a.IdType(), CAircraft::EIdType::ICAO);
            ASSERT_EQUALS(a.AircraftType(), CAircraft::EAircraftType::UFO);
            ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQUALS(a.Location(), (SLocation{2., 2., 2}));
            ASSERT_EQUALS(a.Movement(), (CAircraft::SMovement{1., 1., 1.}));
            ASSERT_EQUALS(a.Timestamp(), CTimestamp("115900"));
            ASSERT_TRUE(a.HasFullInfo());
        }
    });
    test("TryUpdate - prefer flarm", [] {
        CAircraft a(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN, {1., 1., 1},
                    CTimestamp("115800"));
        CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                     {1., 1., 1.}, CTimestamp("115900"));
        ++a;
        ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
        ASSERT_EQUALS(a2.TargetType(), CAircraft::ETargetType::FLARM);
        ASSERT_EQUALS(a.UpdateAge(), 1);
        ASSERT_TRUE(a.TryUpdate(std::move(a2)));
        ASSERT_EQUALS(a.UpdateAge(), 0);
        ASSERT_EQUALS(a.IdType(), CAircraft::EIdType::ICAO);
        ASSERT_EQUALS(a.AircraftType(), CAircraft::EAircraftType::UFO);
        ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
        ASSERT_EQUALS(a.Location(), (SLocation{2., 2., 2}));
        ASSERT_EQUALS(a.Movement(), (CAircraft::SMovement{1., 1., 1.}));
        ASSERT_EQUALS(a.Timestamp(), CTimestamp("115900"));
        ASSERT_TRUE(a.HasFullInfo());
    });
    test("TryUpdate - lower priority", [] {
        {
            CAircraft a(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        {1., 1., 1}, CTimestamp("115800"));
            CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                         {1., 1., 1.}, CTimestamp("115900"));
            a2.TargetType(CAircraft::ETargetType::TRANSPONDER);
            ++a;
            ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQUALS(a.UpdateAge(), 1);
            ASSERT_FALSE(a.TryUpdate(std::move(a2)));
            ASSERT_EQUALS(a.UpdateAge(), 1);
            ASSERT_EQUALS(a.IdType(), CAircraft::EIdType::RANDOM);
            ASSERT_EQUALS(a.AircraftType(), CAircraft::EAircraftType::UNKNOWN);
            ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQUALS(a.Location(), (SLocation{1., 1., 1}));
            ASSERT_EQUALS(a.Movement(), (CAircraft::SMovement{0., 0., 0.}));
            ASSERT_EQUALS(a.Timestamp(), CTimestamp("115800"));
            ASSERT_FALSE(a.HasFullInfo());
        }
        {
            CAircraft a(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        {1., 1., 1}, CTimestamp("115800"));
            CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                         {1., 1., 1.}, CTimestamp("115900"));
            a.TargetType(CAircraft::ETargetType::FLARM);
            ++a;
            ASSERT_EQUALS(a2.TargetType(), CAircraft::ETargetType::FLARM);
            ASSERT_EQUALS(a.UpdateAge(), 1);
            ASSERT_FALSE(a.TryUpdate(std::move(a2)));
            ASSERT_EQUALS(a.UpdateAge(), 1);
            ASSERT_EQUALS(a.IdType(), CAircraft::EIdType::RANDOM);
            ASSERT_EQUALS(a.AircraftType(), CAircraft::EAircraftType::UNKNOWN);
            ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQUALS(a.Location(), (SLocation{1., 1., 1}));
            ASSERT_EQUALS(a.Movement(), (CAircraft::SMovement{0., 0., 0.}));
            ASSERT_EQUALS(a.Timestamp(), CTimestamp("115800"));
            ASSERT_FALSE(a.HasFullInfo());
        }
    });
    test("TryUpdate - older timestamp", [] {
        {
            CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        {1., 1., 1}, CTimestamp("115900"));
            CAircraft a2(1, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                         {1., 1., 1.}, CTimestamp("115800"));
            a2.TargetType(CAircraft::ETargetType::TRANSPONDER);
            ++a;
            ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQUALS(a.UpdateAge(), 1);
            ASSERT_FALSE(a.TryUpdate(std::move(a2)));
            ASSERT_EQUALS(a.UpdateAge(), 1);
            ASSERT_EQUALS(a.IdType(), CAircraft::EIdType::RANDOM);
            ASSERT_EQUALS(a.AircraftType(), CAircraft::EAircraftType::UNKNOWN);
            ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQUALS(a.Location(), (SLocation{1., 1., 1}));
            ASSERT_EQUALS(a.Movement(), (CAircraft::SMovement{0., 0., 0.}));
            ASSERT_EQUALS(a.Timestamp(), CTimestamp("115900"));
            ASSERT_FALSE(a.HasFullInfo());
        }
        {
            CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        {1., 1., 1}, CTimestamp("115900"));
            CAircraft a2(1, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                         {1., 1., 1.}, CTimestamp("115800"));
            a.TargetType(CAircraft::ETargetType::FLARM);
            ++a;
            ASSERT_EQUALS(a2.TargetType(), CAircraft::ETargetType::FLARM);
            ASSERT_EQUALS(a.UpdateAge(), 1);
            ASSERT_FALSE(a.TryUpdate(std::move(a2)));
            ASSERT_EQUALS(a.UpdateAge(), 1);
            ASSERT_EQUALS(a.IdType(), CAircraft::EIdType::RANDOM);
            ASSERT_EQUALS(a.AircraftType(), CAircraft::EAircraftType::UNKNOWN);
            ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQUALS(a.Location(), (SLocation{1., 1., 1}));
            ASSERT_EQUALS(a.Movement(), (CAircraft::SMovement{0., 0., 0.}));
            ASSERT_EQUALS(a.Timestamp(), CTimestamp("115900"));
            ASSERT_FALSE(a.HasFullInfo());
        }
    });
    test("TryUpdate - transponder to flarm", [] {
        CAircraft a(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                    {1., 1., 1.}, CTimestamp("115800"));
        CAircraft a2(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN, {1., 1., 1},
                     CTimestamp("115900"));
        ++a;
        ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::FLARM);
        ASSERT_EQUALS(a2.TargetType(), CAircraft::ETargetType::TRANSPONDER);
        ASSERT_EQUALS(a.UpdateAge(), 1);
        ASSERT_FALSE(a.TryUpdate(std::move(a2)));
        ASSERT_EQUALS(a.UpdateAge(), 1);
        ASSERT_EQUALS(a.IdType(), CAircraft::EIdType::ICAO);
        ASSERT_EQUALS(a.AircraftType(), CAircraft::EAircraftType::UFO);
        ASSERT_EQUALS(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
        ASSERT_EQUALS(a.Location(), (SLocation{2., 2., 2}));
        ASSERT_EQUALS(a.Movement(), (CAircraft::SMovement{1., 1., 1.}));
        ASSERT_EQUALS(a.Timestamp(), CTimestamp("115800"));
        ASSERT_TRUE(a.HasFullInfo());
    });
    test("init - fail", [] {
        ASSERT_THROWS(CAircraft(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                                {SLocation::MIN_LATITUDE - 1., 1., 1}, CTimestamp("115900")),
                      util::error::CLimitsExceededError);
        ASSERT_THROWS(CAircraft(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                                {SLocation::MAX_LATITUDE + 1., 1., 1}, CTimestamp("115900")),
                      util::error::CLimitsExceededError);
        ASSERT_THROWS(CAircraft(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                                {1., SLocation::MIN_LONGITUDE - 1., 1}, CTimestamp("115900")),
                      util::error::CLimitsExceededError);
        ASSERT_THROWS(CAircraft(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                                {1., SLocation::MAX_LONGITUDE + 1., 1}, CTimestamp("115900")),
                      util::error::CLimitsExceededError);
    });
})

TEST_MODULE_PAR(test_timestamp, {
    setup([] {
        date_time::Day(1);
        date_time::Now(12, 0, 0);
    });
    test("creation - valid format", [] {
        ASSERT_NOTHROW(CTimestamp("120000"));
        ASSERT_NOTHROW(CTimestamp("12:00:00.000"));
    });
    test("creation - invalid format", [] {
        ASSERT_THROWS(CTimestamp(""), vfrb::object::error::CTimestampParseError);
        ASSERT_THROWS(CTimestamp("adgjhag"), vfrb::object::error::CTimestampParseError);
        ASSERT_THROWS(CTimestamp("36:60:11.11111"), vfrb::object::error::CTimestampParseError);
        ASSERT_THROWS(CTimestamp("366022"), vfrb::object::error::CTimestampParseError);
    });
    test("comparison - incremental time", [] {
        date_time::Day(1);
        date_time::Now(13, 0, 0);
        CTimestamp const t1("120000");
        CTimestamp const t2("120001");

        ASSERT(t2, GT, t1);
        ASSERT(t1, !GT, t2);
    });
    test("comparison - old day msg", [] {
        date_time::Day(1);
        date_time::Now(23, 59, 0);
        CTimestamp const t1("235800");
        date_time::Day(2);
        date_time::Now(0, 0, 0);
        CTimestamp const t2("235900");

        ASSERT(t2, GT, t1);
        ASSERT(t1, !GT, t2);
    });
    test("comparison - new day msg", [] {
        date_time::Day(1);
        date_time::Now(23, 59, 0);
        CTimestamp const t1("235800");
        date_time::Day(2);
        date_time::Now(0, 1, 0);
        CTimestamp const t2("000000");

        ASSERT(t2, GT, t1);
        ASSERT(t1, !GT, t2);
    });
    test("comparison - incremental day", [] {
        date_time::Day(1);
        date_time::Now(13, 0, 0);
        CTimestamp const t1("120000");
        date_time::Day(2);
        CTimestamp const t2("110000");

        ASSERT(t2, GT, t1);
        ASSERT(t1, !GT, t2);
    });
})
