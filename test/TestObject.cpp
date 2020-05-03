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

#include "object/CAircraft.hpp"
#include "object/CAtmosphere.hpp"
#include "object/CGpsPosition.hpp"
#include "object/CTimestamp.hpp"
#include "object/CWind.hpp"

#include "Helper.hpp"

using namespace vfrb;
using namespace object;

namespace vfrb::object::date_time
{
extern void Now(u32, u32, u32);
extern void Day(s64);
}  // namespace vfrb::object::date_time

DESCRIBE_PAR("test_CWind") {
    IT("should age by one") {
        CWind w;
        ASSERT_EQ(w.UpdateAge(), 0);
        ++w;
        ASSERT_EQ(w.UpdateAge(), 1);
    }
    IT("should update with higher priority") {
        CWind w(0, "1");
        CWind w2(1, "2");
        ++w;
        ASSERT_EQ(w.UpdateAge(), 1);
        ASSERT_TRUE(w.TryUpdate(std::move(w2)));
        ASSERT_EQ(w.UpdateAge(), 0);
        ASSERT_EQ(w.Nmea(), "2");
    }
    IT("should update with equal priority") {
        CWind w(0, "1");
        CWind w2(0, "2");
        ++w;
        ASSERT_EQ(w.UpdateAge(), 1);
        ASSERT_TRUE(w.TryUpdate(std::move(w2)));
        ASSERT_EQ(w.UpdateAge(), 0);
        ASSERT_EQ(w.Nmea(), "2");
    }
    IT("should update outdated object with lower priority") {
        CWind w(1, "1");
        CWind w2(0, "2");
        while ((++w).UpdateAge() < CObject::OUTDATED) {
        };
        ASSERT_EQ(w.UpdateAge(), CObject::OUTDATED);
        ASSERT_TRUE(w.TryUpdate(std::move(w2)));
        ASSERT_EQ(w.UpdateAge(), 0);
        ASSERT_EQ(w.Nmea(), "2");
    }
    IT("should refuse update with lower priority") {
        CWind w(1, "1");
        CWind w2(0, "2");
        ++w;
        ASSERT_EQ(w.UpdateAge(), 1);
        ASSERT_FALSE(w.TryUpdate(std::move(w2)));
        ASSERT_EQ(w.UpdateAge(), 1);
        ASSERT_EQ(w.Nmea(), "1");
    };
};

TEST_MODULE_PAR(test_Atmosphere, {
    IT("aging") {
        CAtmosphere a;
        ASSERT_EQ(a.UpdateAge(), 0);
        ++a;
        ASSERT_EQ(a.UpdateAge(), 1);
    });
    IT("TryUpdate - higher priority") {
        CAtmosphere a(0, "1");
        CAtmosphere a2(1, 1.0, "2");
        ++a;
        ASSERT_EQ(a.UpdateAge(), 1);
        ASSERT_TRUE(a.TryUpdate(std::move(a2)));
        ASSERT_EQ(a.UpdateAge(), 0);
        ASSERT_EQ(a.Nmea(), "2"s);
        ASSERT_EQ(a.Pressure(), 1.0);
    });
    IT("TryUpdate - equal priority") {
        CAtmosphere a(0, "1");
        CAtmosphere a2(0, 1.0, "2");
        ++a;
        ASSERT_EQ(a.UpdateAge(), 1);
        ASSERT_TRUE(a.TryUpdate(std::move(a2)));
        ASSERT_EQ(a.UpdateAge(), 0);
        ASSERT_EQ(a.Nmea(), "2"s);
        ASSERT_EQ(a.Pressure(), 1.0);
    });
    IT("TryUpdate - outdated") {
        CAtmosphere a(1, "1");
        CAtmosphere a2(0, 1.0, "2");
        while ((++a).UpdateAge() < CObject::OUTDATED)
            ;
        ASSERT_EQ(a.UpdateAge(), CObject::OUTDATED);
        ASSERT_TRUE(a.TryUpdate(std::move(a2)));
        ASSERT_EQ(a.UpdateAge(), 0);
        ASSERT_EQ(a.Nmea(), "2"s);
        ASSERT_EQ(a.Pressure(), 1.0);
    });
    IT("TryUpdate - lower priority") {
        CAtmosphere a(1, 2.0, "1");
        CAtmosphere a2(0, 1.0, "2");
        ++a;
        ASSERT_EQ(a.UpdateAge(), 1);
        ASSERT_FALSE(a.TryUpdate(std::move(a2)));
        ASSERT_EQ(a.UpdateAge(), 1);
        ASSERT_EQ(a.Nmea(), "1"s);
        ASSERT_EQ(a.Pressure(), 2.0);
    });
    IT("init - fail") {
        ASSERT_THROWS(CAtmosphere(0, -1.0, ""), util::error::CLimitsExceededError);
        ASSERT_THROWS(CAtmosphere(0, 2001.0, ""), util::error::CLimitsExceededError);
    });
})

TEST_MODULE_PAR(test_GpsPosition, {
    setup([] {
        date_time::Day(1);
        date_time::Now(12, 0, 0);
    });
    IT("aging") {
        CGpsPosition p(0, {1., 1., 1}, 1.);
        ASSERT_EQ(p.UpdateAge(), 0);
        ++p;
        ASSERT_EQ(p.UpdateAge(), 1);
    });
    IT("TryUpdate - higher priority") {
        CGpsPosition p(0, {1., 1., 1}, 1.);
        CGpsPosition p2(1, {2., 2., 2}, 2., 2., 4, 6, CTimestamp("115900"));
        ++p;
        ASSERT_EQ(p.UpdateAge(), 1);
        ASSERT_TRUE(p.TryUpdate(std::move(p2)));
        ASSERT_EQ(p.UpdateAge(), 0);
        ASSERT_EQ(p.Geoid(), 2.);
        ASSERT_EQ(p.Dilution(), 2.);
        ASSERT_EQ(p.FixQuality(), 6);
        ASSERT_EQ(p.NrOfSatellites(), 4);
        ASSERT_EQ(p.Location(), (SLocation{2., 2., 2}));
    });
    IT("TryUpdate - equal priority") {
        CGpsPosition p(0, {1., 1., 1}, 1.);
        CGpsPosition p2(0, {2., 2., 2}, 2., 2., 4, 6, CTimestamp("115900"));
        ++p;
        ASSERT_EQ(p.UpdateAge(), 1);
        ASSERT_TRUE(p.TryUpdate(std::move(p2)));
        ASSERT_EQ(p.UpdateAge(), 0);
        ASSERT_EQ(p.Geoid(), 2.);
        ASSERT_EQ(p.Dilution(), 2.);
        ASSERT_EQ(p.FixQuality(), 6);
        ASSERT_EQ(p.NrOfSatellites(), 4);
        ASSERT_EQ(p.Location(), (SLocation{2., 2., 2}));
    });
    IT("TryUpdate - outdated") {
        CGpsPosition p(1, {1., 1., 1}, 1.);
        CGpsPosition p2(0, {2., 2., 2}, 2., 2., 4, 6, CTimestamp("115900"));
        while ((++p).UpdateAge() < CObject::OUTDATED)
            ;
        ASSERT_EQ(p.UpdateAge(), CObject::OUTDATED);
        ASSERT_TRUE(p.TryUpdate(std::move(p2)));
        ASSERT_EQ(p.UpdateAge(), 0);
        ASSERT_EQ(p.Geoid(), 2.);
        ASSERT_EQ(p.Dilution(), 2.);
        ASSERT_EQ(p.FixQuality(), 6);
        ASSERT_EQ(p.NrOfSatellites(), 4);
        ASSERT_EQ(p.Location(), (SLocation{2., 2., 2}));
    });
    IT("TryUpdate - lower priority") {
        CGpsPosition p(1, {1., 1., 1}, 1., 1., 3, 5, CTimestamp("115800"));
        CGpsPosition p2(0, {2., 2., 2}, 2., 2., 4, 6, CTimestamp("115900"));
        ++p;
        ASSERT_EQ(p.UpdateAge(), 1);
        ASSERT_FALSE(p.TryUpdate(std::move(p2)));
        ASSERT_EQ(p.UpdateAge(), 1);
        ASSERT_EQ(p.Geoid(), 1.);
        ASSERT_EQ(p.Dilution(), 1.);
        ASSERT_EQ(p.FixQuality(), 5);
        ASSERT_EQ(p.NrOfSatellites(), 3);
        ASSERT_EQ(p.Location(), (SLocation{1., 1., 1}));
    });
    IT("TryUpdate - older timestamp") {
        CGpsPosition p(0, {1., 1., 1}, 1., 1., 3, 5, CTimestamp("115900"));
        CGpsPosition p2(1, {2., 2., 2}, 2., 2., 4, 6, CTimestamp("115800"));
        ++p;
        ASSERT_EQ(p.UpdateAge(), 1);
        ASSERT_FALSE(p.TryUpdate(std::move(p2)));
        ASSERT_EQ(p.UpdateAge(), 1);
        ASSERT_EQ(p.Geoid(), 1.);
        ASSERT_EQ(p.Dilution(), 1.);
        ASSERT_EQ(p.FixQuality(), 5);
        ASSERT_EQ(p.NrOfSatellites(), 3);
        ASSERT_EQ(p.Location(), (SLocation{1., 1., 1}));
    });
    IT("init - fail") {
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
    IT("aging") {
        CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN, {1., 1., 1},
                    CTimestamp("115900"));
        ASSERT_EQ(a.UpdateAge(), 0);
        ++a;
        ASSERT_EQ(a.UpdateAge(), 1);
    });
    IT("TryUpdate - higher priority"){
        {CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN, {1., 1., 1},
                     CTimestamp("115800"));
    CAircraft a2(1, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                 {1., 1., 1.}, CTimestamp("115900"));
    a2.TargetType(CAircraft::ETargetType::TRANSPONDER);
    ++a;
    ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
    ASSERT_EQ(a.UpdateAge(), 1);
    ASSERT_TRUE(a.TryUpdate(std::move(a2)));
    ASSERT_EQ(a.UpdateAge(), 0);
    ASSERT_EQ(a.IdType(), CAircraft::EIdType::ICAO);
    ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UFO);
    ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
    ASSERT_EQ(a.Location(), (SLocation{2., 2., 2}));
    ASSERT_EQ(a.Movement(), (CAircraft::SMovement{1., 1., 1.}));
    ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
    ASSERT_TRUE(a.HasFullInfo());
        }
        {
    CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN, {1., 1., 1},
                CTimestamp("115800"));
    CAircraft a2(1, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                 {1., 1., 1.}, CTimestamp("115900"));
    a.TargetType(CAircraft::ETargetType::FLARM);
    ++a;
    ASSERT_EQ(a2.TargetType(), CAircraft::ETargetType::FLARM);
    ASSERT_EQ(a.UpdateAge(), 1);
    ASSERT_TRUE(a.TryUpdate(std::move(a2)));
    ASSERT_EQ(a.UpdateAge(), 0);
    ASSERT_EQ(a.IdType(), CAircraft::EIdType::ICAO);
    ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UFO);
    ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
    ASSERT_EQ(a.Location(), (SLocation{2., 2., 2}));
    ASSERT_EQ(a.Movement(), (CAircraft::SMovement{1., 1., 1.}));
    ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
    ASSERT_TRUE(a.HasFullInfo());
        }
});
IT("TryUpdate - equal priority"){
    {CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN, {1., 1., 1},
                 CTimestamp("115800"));
CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2}, {1., 1., 1.},
             CTimestamp("115900"));
a2.TargetType(CAircraft::ETargetType::TRANSPONDER);
++a;
ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
ASSERT_EQ(a.UpdateAge(), 1);
ASSERT_TRUE(a.TryUpdate(std::move(a2)));
ASSERT_EQ(a.UpdateAge(), 0);
ASSERT_EQ(a.IdType(), CAircraft::EIdType::ICAO);
ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UFO);
ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
ASSERT_EQ(a.Location(), (SLocation{2., 2., 2}));
ASSERT_EQ(a.Movement(), (CAircraft::SMovement{1., 1., 1.}));
ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
ASSERT_TRUE(a.HasFullInfo());
}
{
    CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN, {1., 1., 1},
                CTimestamp("115800"));
    CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                 {1., 1., 1.}, CTimestamp("115900"));
    a.TargetType(CAircraft::ETargetType::FLARM);
    ++a;
    ASSERT_EQ(a2.TargetType(), CAircraft::ETargetType::FLARM);
    ASSERT_EQ(a.UpdateAge(), 1);
    ASSERT_TRUE(a.TryUpdate(std::move(a2)));
    ASSERT_EQ(a.UpdateAge(), 0);
    ASSERT_EQ(a.IdType(), CAircraft::EIdType::ICAO);
    ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UFO);
    ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
    ASSERT_EQ(a.Location(), (SLocation{2., 2., 2}));
    ASSERT_EQ(a.Movement(), (CAircraft::SMovement{1., 1., 1.}));
    ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
    ASSERT_TRUE(a.HasFullInfo());
}
});
IT("TryUpdate - outdated"){{CAircraft a(1, "#00001", CAircraft::EIdType::RANDOM,
                                        CAircraft::EAircraftType::UNKNOWN, {1., 1., 1}, CTimestamp("115800"));
CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2}, {1., 1., 1.},
             CTimestamp("115900"));
a2.TargetType(CAircraft::ETargetType::TRANSPONDER);
while ((++a).UpdateAge() < CObject::OUTDATED)
    ;
ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
ASSERT_EQ(a.UpdateAge(), CObject::OUTDATED);
ASSERT_TRUE(a.TryUpdate(std::move(a2)));
ASSERT_EQ(a.UpdateAge(), 0);
ASSERT_EQ(a.IdType(), CAircraft::EIdType::ICAO);
ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UFO);
ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
ASSERT_EQ(a.Location(), (SLocation{2., 2., 2}));
ASSERT_EQ(a.Movement(), (CAircraft::SMovement{1., 1., 1.}));
ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
ASSERT_TRUE(a.HasFullInfo());
}
{
    CAircraft a(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN, {1., 1., 1},
                CTimestamp("115800"));
    CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                 {1., 1., 1.}, CTimestamp("115900"));
    a.TargetType(CAircraft::ETargetType::FLARM);
    while ((++a).UpdateAge() < CObject::OUTDATED)
        ;
    ASSERT_EQ(a2.TargetType(), CAircraft::ETargetType::FLARM);
    ASSERT_EQ(a.UpdateAge(), CObject::OUTDATED);
    ASSERT_TRUE(a.TryUpdate(std::move(a2)));
    ASSERT_EQ(a.UpdateAge(), 0);
    ASSERT_EQ(a.IdType(), CAircraft::EIdType::ICAO);
    ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UFO);
    ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
    ASSERT_EQ(a.Location(), (SLocation{2., 2., 2}));
    ASSERT_EQ(a.Movement(), (CAircraft::SMovement{1., 1., 1.}));
    ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
    ASSERT_TRUE(a.HasFullInfo());
}
});
IT("TryUpdate - prefer flarm") {
    CAircraft a(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN, {1., 1., 1},
                CTimestamp("115800"));
    CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                 {1., 1., 1.}, CTimestamp("115900"));
    ++a;
    ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
    ASSERT_EQ(a2.TargetType(), CAircraft::ETargetType::FLARM);
    ASSERT_EQ(a.UpdateAge(), 1);
    ASSERT_TRUE(a.TryUpdate(std::move(a2)));
    ASSERT_EQ(a.UpdateAge(), 0);
    ASSERT_EQ(a.IdType(), CAircraft::EIdType::ICAO);
    ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UFO);
    ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
    ASSERT_EQ(a.Location(), (SLocation{2., 2., 2}));
    ASSERT_EQ(a.Movement(), (CAircraft::SMovement{1., 1., 1.}));
    ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
    ASSERT_TRUE(a.HasFullInfo());
});
IT("TryUpdate - lower priority"){
    {CAircraft a(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN, {1., 1., 1},
                 CTimestamp("115800"));
CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2}, {1., 1., 1.},
             CTimestamp("115900"));
a2.TargetType(CAircraft::ETargetType::TRANSPONDER);
++a;
ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
ASSERT_EQ(a.UpdateAge(), 1);
ASSERT_FALSE(a.TryUpdate(std::move(a2)));
ASSERT_EQ(a.UpdateAge(), 1);
ASSERT_EQ(a.IdType(), CAircraft::EIdType::RANDOM);
ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UNKNOWN);
ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
ASSERT_EQ(a.Location(), (SLocation{1., 1., 1}));
ASSERT_EQ(a.Movement(), (CAircraft::SMovement{0., 0., 0.}));
ASSERT_EQ(a.Timestamp(), CTimestamp("115800"));
ASSERT_FALSE(a.HasFullInfo());
}
{
    CAircraft a(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN, {1., 1., 1},
                CTimestamp("115800"));
    CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                 {1., 1., 1.}, CTimestamp("115900"));
    a.TargetType(CAircraft::ETargetType::FLARM);
    ++a;
    ASSERT_EQ(a2.TargetType(), CAircraft::ETargetType::FLARM);
    ASSERT_EQ(a.UpdateAge(), 1);
    ASSERT_FALSE(a.TryUpdate(std::move(a2)));
    ASSERT_EQ(a.UpdateAge(), 1);
    ASSERT_EQ(a.IdType(), CAircraft::EIdType::RANDOM);
    ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UNKNOWN);
    ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
    ASSERT_EQ(a.Location(), (SLocation{1., 1., 1}));
    ASSERT_EQ(a.Movement(), (CAircraft::SMovement{0., 0., 0.}));
    ASSERT_EQ(a.Timestamp(), CTimestamp("115800"));
    ASSERT_FALSE(a.HasFullInfo());
}
});
IT("TryUpdate - older timestamp"){
    {CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN, {1., 1., 1},
                 CTimestamp("115900"));
CAircraft a2(1, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2}, {1., 1., 1.},
             CTimestamp("115800"));
a2.TargetType(CAircraft::ETargetType::TRANSPONDER);
++a;
ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
ASSERT_EQ(a.UpdateAge(), 1);
ASSERT_FALSE(a.TryUpdate(std::move(a2)));
ASSERT_EQ(a.UpdateAge(), 1);
ASSERT_EQ(a.IdType(), CAircraft::EIdType::RANDOM);
ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UNKNOWN);
ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
ASSERT_EQ(a.Location(), (SLocation{1., 1., 1}));
ASSERT_EQ(a.Movement(), (CAircraft::SMovement{0., 0., 0.}));
ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
ASSERT_FALSE(a.HasFullInfo());
}
{
    CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN, {1., 1., 1},
                CTimestamp("115900"));
    CAircraft a2(1, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                 {1., 1., 1.}, CTimestamp("115800"));
    a.TargetType(CAircraft::ETargetType::FLARM);
    ++a;
    ASSERT_EQ(a2.TargetType(), CAircraft::ETargetType::FLARM);
    ASSERT_EQ(a.UpdateAge(), 1);
    ASSERT_FALSE(a.TryUpdate(std::move(a2)));
    ASSERT_EQ(a.UpdateAge(), 1);
    ASSERT_EQ(a.IdType(), CAircraft::EIdType::RANDOM);
    ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UNKNOWN);
    ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
    ASSERT_EQ(a.Location(), (SLocation{1., 1., 1}));
    ASSERT_EQ(a.Movement(), (CAircraft::SMovement{0., 0., 0.}));
    ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
    ASSERT_FALSE(a.HasFullInfo());
}
});
IT("TryUpdate - transponder to flarm") {
    CAircraft a(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO, {2., 2., 2},
                {1., 1., 1.}, CTimestamp("115800"));
    CAircraft a2(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN, {1., 1., 1},
                 CTimestamp("115900"));
    ++a;
    ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::FLARM);
    ASSERT_EQ(a2.TargetType(), CAircraft::ETargetType::TRANSPONDER);
    ASSERT_EQ(a.UpdateAge(), 1);
    ASSERT_FALSE(a.TryUpdate(std::move(a2)));
    ASSERT_EQ(a.UpdateAge(), 1);
    ASSERT_EQ(a.IdType(), CAircraft::EIdType::ICAO);
    ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UFO);
    ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
    ASSERT_EQ(a.Location(), (SLocation{2., 2., 2}));
    ASSERT_EQ(a.Movement(), (CAircraft::SMovement{1., 1., 1.}));
    ASSERT_EQ(a.Timestamp(), CTimestamp("115800"));
    ASSERT_TRUE(a.HasFullInfo());
});
IT("init - fail") {
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
    IT("creation - valid format") {
        ASSERT_NOTHROW(CTimestamp("120000"));
        ASSERT_NOTHROW(CTimestamp("12:00:00.000"));
    });
    IT("creation - invalid format") {
        ASSERT_THROWS(CTimestamp(""), vfrb::object::error::CTimestampParseError);
        ASSERT_THROWS(CTimestamp("adgjhag"), vfrb::object::error::CTimestampParseError);
        ASSERT_THROWS(CTimestamp("36:60:11.11111"), vfrb::object::error::CTimestampParseError);
        ASSERT_THROWS(CTimestamp("366022"), vfrb::object::error::CTimestampParseError);
    });
    IT("comparison - incremental time") {
        date_time::Day(1);
        date_time::Now(13, 0, 0);
        CTimestamp const t1("120000");
        CTimestamp const t2("120001");

        ASSERT(t2, GT, t1);
        ASSERT(t1, !GT, t2);
    });
    IT("comparison - old day msg") {
        date_time::Day(1);
        date_time::Now(23, 59, 0);
        CTimestamp const t1("235800");
        date_time::Day(2);
        date_time::Now(0, 0, 0);
        CTimestamp const t2("235900");

        ASSERT(t2, GT, t1);
        ASSERT(t1, !GT, t2);
    });
    IT("comparison - new day msg") {
        date_time::Day(1);
        date_time::Now(23, 59, 0);
        CTimestamp const t1("235800");
        date_time::Day(2);
        date_time::Now(0, 1, 0);
        CTimestamp const t2("000000");

        ASSERT(t2, GT, t1);
        ASSERT(t1, !GT, t2);
    });
    IT("comparison - incremental day") {
        date_time::Day(1);
        date_time::Now(13, 0, 0);
        CTimestamp const t1("120000");
        date_time::Day(2);
        CTimestamp const t2("110000");

        ASSERT(t2, GT, t1);
        ASSERT(t1, !GT, t2);
    });
})
