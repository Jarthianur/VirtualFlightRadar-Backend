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
#include "util/Bounds.hpp"

#include "Helper.hpp"

using namespace vfrb;
using namespace object;

namespace vfrb::object::date_time
{
extern void Now(u32, u32, u32);
extern void Day(i64);
}  // namespace vfrb::object::date_time

DESCRIBE_PAR("test_CWind") {
    IT("should age by one") {
        CWind w;
        ASSERT_EQ(w.UpdateAge(), 0U);
        ++w;
        ASSERT_EQ(w.UpdateAge(), 1U);
    }
    IT("should update with higher priority") {
        CWind w(0, "1");
        CWind w2(1, "2");
        ++w;
        ASSERT_EQ(w.UpdateAge(), 1U);
        ASSERT_TRUE(w.TryUpdate(std::move(w2)));
        ASSERT_EQ(w.UpdateAge(), 0U);
        ASSERT_EQ(w.Nmea(), "2");
    }
    IT("should update with equal priority") {
        CWind w(0, "1");
        CWind w2(0, "2");
        ++w;
        ASSERT_EQ(w.UpdateAge(), 1U);
        ASSERT_TRUE(w.TryUpdate(std::move(w2)));
        ASSERT_EQ(w.UpdateAge(), 0U);
        ASSERT_EQ(w.Nmea(), "2");
    }
    IT("should update outdated object with lower priority") {
        CWind w(1, "1");
        CWind w2(0, "2");
        while ((++w).UpdateAge() < CObject::OUTDATED) {
        };
        ASSERT_EQ(w.UpdateAge(), CObject::OUTDATED);
        ASSERT_TRUE(w.TryUpdate(std::move(w2)));
        ASSERT_EQ(w.UpdateAge(), 0U);
        ASSERT_EQ(w.Nmea(), "2");
    }
    IT("should refuse update with lower priority") {
        CWind w(1, "1");
        CWind w2(0, "2");
        ++w;
        ASSERT_EQ(w.UpdateAge(), 1U);
        ASSERT_FALSE(w.TryUpdate(std::move(w2)));
        ASSERT_EQ(w.UpdateAge(), 1U);
        ASSERT_EQ(w.Nmea(), "1");
    };
};

DESCRIBE_PAR("test_CAtmosphere") {
    IT("should age by one") {
        CAtmosphere a;
        ASSERT_EQ(a.UpdateAge(), 0U);
        ++a;
        ASSERT_EQ(a.UpdateAge(), 1U);
    }
    IT("should update with higher priority") {
        CAtmosphere a(0, "1");
        CAtmosphere a2(1, 1.0, "2");
        ++a;
        ASSERT_EQ(a.UpdateAge(), 1U);
        ASSERT_TRUE(a.TryUpdate(std::move(a2)));
        ASSERT_EQ(a.UpdateAge(), 0U);
        ASSERT_EQ(a.Nmea(), "2");
        ASSERT_EQ(a.Pressure(), 1.0);
    }
    IT("should update with equal priority") {
        CAtmosphere a(0, "1");
        CAtmosphere a2(0, 1.0, "2");
        ++a;
        ASSERT_EQ(a.UpdateAge(), 1U);
        ASSERT_TRUE(a.TryUpdate(std::move(a2)));
        ASSERT_EQ(a.UpdateAge(), 0U);
        ASSERT_EQ(a.Nmea(), "2");
        ASSERT_EQ(a.Pressure(), 1.0);
    }
    IT("should update outdated object with lower priority") {
        CAtmosphere a(1, "1");
        CAtmosphere a2(0, 1.0, "2");
        while ((++a).UpdateAge() < CObject::OUTDATED) {
        };
        ASSERT_EQ(a.UpdateAge(), CObject::OUTDATED);
        ASSERT_TRUE(a.TryUpdate(std::move(a2)));
        ASSERT_EQ(a.UpdateAge(), 0U);
        ASSERT_EQ(a.Nmea(), "2");
        ASSERT_EQ(a.Pressure(), 1.0);
    }
    IT("should refuse update with lower priority") {
        CAtmosphere a(1, 2.0, "1");
        CAtmosphere a2(0, 1.0, "2");
        ++a;
        ASSERT_EQ(a.UpdateAge(), 1U);
        ASSERT_FALSE(a.TryUpdate(std::move(a2)));
        ASSERT_EQ(a.UpdateAge(), 1U);
        ASSERT_EQ(a.Nmea(), "1");
        ASSERT_EQ(a.Pressure(), 2.0);
    }
    IT("should throw if given values are outside acceptable range") {
        ASSERT_THROWS(CAtmosphere(0, -1.0, ""), util::error::CBoundsExceededError);
        ASSERT_THROWS(CAtmosphere(0, 2001.0, ""), util::error::CBoundsExceededError);
    };
};

DESCRIBE_PAR("test_CGpsPosition") {
    SETUP() {
        date_time::Day(1);
        date_time::Now(12, 0, 0);
    }

    IT("should age by one") {
        CGpsPosition p(0, {1., 1., 1}, 1., 1., 3, 5, CTimestamp("115800"));
        ASSERT_EQ(p.UpdateAge(), 0U);
        ++p;
        ASSERT_EQ(p.UpdateAge(), 1U);
    }
    IT("should update with higher priority") {
        CGpsPosition p(0, {1., 1., 1}, 1., 1., 3, 5, CTimestamp("115800"));
        CGpsPosition p2(1, {2., 2., 2}, 2., 2., 4, 6, CTimestamp("115900"));
        ++p;
        ASSERT_EQ(p.UpdateAge(), 1U);
        ASSERT_TRUE(p.TryUpdate(std::move(p2)));
        ASSERT_EQ(p.UpdateAge(), 0U);
        ASSERT_EQ(p.Geoid(), 2.);
        ASSERT_EQ(p.Dilution(), 2.);
        ASSERT_EQ(p.FixQuality(), 6);
        ASSERT_EQ(p.NrOfSatellites(), 4);
        ASSERT_EQ(p.Location().Latitude, 2.);
        ASSERT_EQ(p.Location().Longitude, 2.);
        ASSERT_EQ(p.Location().Altitude, 2);
    }
    IT("should update with equal priority") {
        CGpsPosition p(0, {1., 1., 1}, 1., 1., 3, 5, CTimestamp("115800"));
        CGpsPosition p2(0, {2., 2., 2}, 2., 2., 4, 6, CTimestamp("115900"));
        ++p;
        ASSERT_EQ(p.UpdateAge(), 1U);
        ASSERT_TRUE(p.TryUpdate(std::move(p2)));
        ASSERT_EQ(p.UpdateAge(), 0U);
        ASSERT_EQ(p.Geoid(), 2.);
        ASSERT_EQ(p.Dilution(), 2.);
        ASSERT_EQ(p.FixQuality(), 6);
        ASSERT_EQ(p.NrOfSatellites(), 4);
        ASSERT_EQ(p.Location().Latitude, 2.);
        ASSERT_EQ(p.Location().Longitude, 2.);
        ASSERT_EQ(p.Location().Altitude, 2);
    }
    IT("should update outdated object with lower priority") {
        CGpsPosition p(1, {1., 1., 1}, 1., 1., 3, 5, CTimestamp("115800"));
        CGpsPosition p2(0, {2., 2., 2}, 2., 2., 4, 6, CTimestamp("115900"));
        while ((++p).UpdateAge() < CObject::OUTDATED) {
        };
        ASSERT_EQ(p.UpdateAge(), CObject::OUTDATED);
        ASSERT_TRUE(p.TryUpdate(std::move(p2)));
        ASSERT_EQ(p.UpdateAge(), 0U);
        ASSERT_EQ(p.Geoid(), 2.);
        ASSERT_EQ(p.Dilution(), 2.);
        ASSERT_EQ(p.FixQuality(), 6);
        ASSERT_EQ(p.NrOfSatellites(), 4);
        ASSERT_EQ(p.Location().Latitude, 2.);
        ASSERT_EQ(p.Location().Longitude, 2.);
        ASSERT_EQ(p.Location().Altitude, 2);
    }
    IT("should refuse update with lower priority") {
        CGpsPosition p(1, {1., 1., 1}, 1., 1., 3, 5, CTimestamp("115800"));
        CGpsPosition p2(0, {2., 2., 2}, 2., 2., 4, 6, CTimestamp("115900"));
        ++p;
        ASSERT_EQ(p.UpdateAge(), 1U);
        ASSERT_FALSE(p.TryUpdate(std::move(p2)));
        ASSERT_EQ(p.UpdateAge(), 1U);
        ASSERT_EQ(p.Geoid(), 1.);
        ASSERT_EQ(p.Dilution(), 1.);
        ASSERT_EQ(p.FixQuality(), 5);
        ASSERT_EQ(p.NrOfSatellites(), 3);
        ASSERT_EQ(p.Location().Latitude, 1.);
        ASSERT_EQ(p.Location().Longitude, 1.);
        ASSERT_EQ(p.Location().Altitude, 1);
    }
    IT("should refuse update with older timestamp") {
        CGpsPosition p(0, {1., 1., 1}, 1., 1., 3, 5, CTimestamp("115900"));
        CGpsPosition p2(1, {2., 2., 2}, 2., 2., 4, 6, CTimestamp("115800"));
        ++p;
        ASSERT_EQ(p.UpdateAge(), 1U);
        ASSERT_FALSE(p.TryUpdate(std::move(p2)));
        ASSERT_EQ(p.UpdateAge(), 1U);
        ASSERT_EQ(p.Geoid(), 1.);
        ASSERT_EQ(p.Dilution(), 1.);
        ASSERT_EQ(p.FixQuality(), 5);
        ASSERT_EQ(p.NrOfSatellites(), 3);
        ASSERT_EQ(p.Location().Latitude, 1.);
        ASSERT_EQ(p.Location().Longitude, 1.);
        ASSERT_EQ(p.Location().Altitude, 1);
    }
    IT("should throw if given values are outside acceptable range") {
        ASSERT_THROWS(
            CGpsPosition p(0, {SLocation::MIN_LATITUDE - 1., 1., 1}, 1., 1., 3, 5, CTimestamp("115800"));
            , util::error::CBoundsExceededError);
        ASSERT_THROWS(
            CGpsPosition p(0, {SLocation::MAX_LATITUDE + 1., 1., 1}, 1., 1., 3, 5, CTimestamp("115800"));
            , util::error::CBoundsExceededError);
        ASSERT_THROWS(
            CGpsPosition p(0, {1., SLocation::MIN_LONGITUDE - 1., 1}, 1., 1., 3, 5, CTimestamp("115800"));
            , util::error::CBoundsExceededError);
        ASSERT_THROWS(
            CGpsPosition p(0, {1., SLocation::MAX_LONGITUDE + 1., 1}, 1., 1., 3, 5, CTimestamp("115800"));
            , util::error::CBoundsExceededError);
        ASSERT_THROWS(
            CGpsPosition p(0, {1., 1., SLocation::MIN_ALTITUDE - 1}, 1., 1., 3, 5, CTimestamp("115800"));
            , util::error::CBoundsExceededError);
        ASSERT_THROWS(
            CGpsPosition p(0, {1., 1., SLocation::MAX_ALTITUDE + 1}, 1., 1., 3, 5, CTimestamp("115800"));
            , util::error::CBoundsExceededError);
    };
};

DESCRIBE_PAR("test_CAircraft") {
    SETUP() {
        date_time::Day(1);
        date_time::Now(12, 0, 0);
    }

    IT("should age by one") {
        CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {1., 1., 1}, {.0, .0, .0}, CTimestamp("115900"));
        ASSERT_EQ(a.UpdateAge(), 0U);
        ++a;
        ASSERT_EQ(a.UpdateAge(), 1U);
    }
    IT("should update with higher priority") {
        {
            CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::TRANSPONDER, {1., 1., 1}, {.0, .0, .0}, CTimestamp("115800"));
            CAircraft a2(1, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO,
                         CAircraft::ETargetType::TRANSPONDER, {2., 2., 2}, {1., 1., 1.},
                         CTimestamp("115900"));
            ++a;
            ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQ(a.UpdateAge(), 1U);
            ASSERT_TRUE(a.TryUpdate(std::move(a2)));
            ASSERT_EQ(a.UpdateAge(), 0U);
            ASSERT_EQ(a.IdType(), CAircraft::EIdType::ICAO);
            ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UFO);
            ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQ(a.Location().Latitude, 2.);
            ASSERT_EQ(a.Location().Longitude, 2.);
            ASSERT_EQ(a.Location().Altitude, 2);
            ASSERT_EQ(a.Movement().ClimbRate, 1.);
            ASSERT_EQ(a.Movement().GndSpeed, 1.);
            ASSERT_EQ(a.Movement().Heading, 1.);
            ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
        }
        {
            CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::FLARM, {1., 1., 1}, {.0, .0, .0}, CTimestamp("115800"));
            CAircraft a2(1, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO,
                         CAircraft::ETargetType::FLARM, {2., 2., 2}, {1., 1., 1.}, CTimestamp("115900"));
            ++a;
            ASSERT_EQ(a2.TargetType(), CAircraft::ETargetType::FLARM);
            ASSERT_EQ(a.UpdateAge(), 1U);
            ASSERT_TRUE(a.TryUpdate(std::move(a2)));
            ASSERT_EQ(a.UpdateAge(), 0U);
            ASSERT_EQ(a.IdType(), CAircraft::EIdType::ICAO);
            ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UFO);
            ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::FLARM);
            ASSERT_EQ(a.Location().Latitude, 2.);
            ASSERT_EQ(a.Location().Longitude, 2.);
            ASSERT_EQ(a.Location().Altitude, 2);
            ASSERT_EQ(a.Movement().ClimbRate, 1.);
            ASSERT_EQ(a.Movement().GndSpeed, 1.);
            ASSERT_EQ(a.Movement().Heading, 1.);
            ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
        }
    }
    IT("should update with equal priority") {
        {
            CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::TRANSPONDER, {1., 1., 1}, {.0, .0, .0}, CTimestamp("115800"));
            CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO,
                         CAircraft::ETargetType::TRANSPONDER, {2., 2., 2}, {1., 1., 1.},
                         CTimestamp("115900"));
            ++a;
            ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQ(a.UpdateAge(), 1U);
            ASSERT_TRUE(a.TryUpdate(std::move(a2)));
            ASSERT_EQ(a.UpdateAge(), 0U);
            ASSERT_EQ(a.IdType(), CAircraft::EIdType::ICAO);
            ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UFO);
            ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQ(a.Location().Latitude, 2.);
            ASSERT_EQ(a.Location().Longitude, 2.);
            ASSERT_EQ(a.Location().Altitude, 2);
            ASSERT_EQ(a.Movement().ClimbRate, 1.);
            ASSERT_EQ(a.Movement().GndSpeed, 1.);
            ASSERT_EQ(a.Movement().Heading, 1.);
            ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
        }
        {
            CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::FLARM, {1., 1., 1}, {.0, .0, .0}, CTimestamp("115800"));
            CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO,
                         CAircraft::ETargetType::FLARM, {2., 2., 2}, {1., 1., 1.}, CTimestamp("115900"));
            ++a;
            ASSERT_EQ(a2.TargetType(), CAircraft::ETargetType::FLARM);
            ASSERT_EQ(a.UpdateAge(), 1U);
            ASSERT_TRUE(a.TryUpdate(std::move(a2)));
            ASSERT_EQ(a.UpdateAge(), 0U);
            ASSERT_EQ(a.IdType(), CAircraft::EIdType::ICAO);
            ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UFO);
            ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::FLARM);
            ASSERT_EQ(a.Location().Latitude, 2.);
            ASSERT_EQ(a.Location().Longitude, 2.);
            ASSERT_EQ(a.Location().Altitude, 2);
            ASSERT_EQ(a.Movement().ClimbRate, 1.);
            ASSERT_EQ(a.Movement().GndSpeed, 1.);
            ASSERT_EQ(a.Movement().Heading, 1.);
            ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
        }
    }
    IT("should update outdated object with lower priority") {
        {
            CAircraft a(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::TRANSPONDER, {1., 1., 1}, {.0, .0, .0}, CTimestamp("115800"));
            CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO,
                         CAircraft::ETargetType::TRANSPONDER, {2., 2., 2}, {1., 1., 1.},
                         CTimestamp("115900"));
            while ((++a).UpdateAge() < CObject::OUTDATED) {
            };
            ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQ(a.UpdateAge(), CObject::OUTDATED);
            ASSERT_TRUE(a.TryUpdate(std::move(a2)));
            ASSERT_EQ(a.UpdateAge(), 0U);
            ASSERT_EQ(a.IdType(), CAircraft::EIdType::ICAO);
            ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UFO);
            ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQ(a.Location().Latitude, 2.);
            ASSERT_EQ(a.Location().Longitude, 2.);
            ASSERT_EQ(a.Location().Altitude, 2);
            ASSERT_EQ(a.Movement().ClimbRate, 1.);
            ASSERT_EQ(a.Movement().GndSpeed, 1.);
            ASSERT_EQ(a.Movement().Heading, 1.);
            ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
        }
        {
            CAircraft a(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::FLARM, {1., 1., 1}, {.0, .0, .0}, CTimestamp("115800"));
            CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO,
                         CAircraft::ETargetType::FLARM, {2., 2., 2}, {1., 1., 1.}, CTimestamp("115900"));
            while ((++a).UpdateAge() < CObject::OUTDATED) {
            };
            ASSERT_EQ(a2.TargetType(), CAircraft::ETargetType::FLARM);
            ASSERT_EQ(a.UpdateAge(), CObject::OUTDATED);
            ASSERT_TRUE(a.TryUpdate(std::move(a2)));
            ASSERT_EQ(a.UpdateAge(), 0U);
            ASSERT_EQ(a.IdType(), CAircraft::EIdType::ICAO);
            ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UFO);
            ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::FLARM);
            ASSERT_EQ(a.Location().Latitude, 2.);
            ASSERT_EQ(a.Location().Longitude, 2.);
            ASSERT_EQ(a.Location().Altitude, 2);
            ASSERT_EQ(a.Movement().ClimbRate, 1.);
            ASSERT_EQ(a.Movement().GndSpeed, 1.);
            ASSERT_EQ(a.Movement().Heading, 1.);
            ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
        }
    }
    IT("should prefer flarm for update") {
        CAircraft a(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {1., 1., 1}, {.0, .0, .0}, CTimestamp("115800"));
        CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO,
                     CAircraft::ETargetType::FLARM, {2., 2., 2}, {1., 1., 1.}, CTimestamp("115900"));
        ++a;
        ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
        ASSERT_EQ(a2.TargetType(), CAircraft::ETargetType::FLARM);
        ASSERT_EQ(a.UpdateAge(), 1U);
        ASSERT_TRUE(a.TryUpdate(std::move(a2)));
        ASSERT_EQ(a.UpdateAge(), 0U);
        ASSERT_EQ(a.IdType(), CAircraft::EIdType::ICAO);
        ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UFO);
        ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::FLARM);
        ASSERT_EQ(a.Location().Latitude, 2.);
        ASSERT_EQ(a.Location().Longitude, 2.);
        ASSERT_EQ(a.Location().Altitude, 2);
        ASSERT_EQ(a.Movement().ClimbRate, 1.);
        ASSERT_EQ(a.Movement().GndSpeed, 1.);
        ASSERT_EQ(a.Movement().Heading, 1.);
        ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
    }
    IT("should refuse update with lower priority") {
        {
            CAircraft a(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::TRANSPONDER, {1., 1., 1}, {.0, .0, .0}, CTimestamp("115800"));
            CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO,
                         CAircraft::ETargetType::TRANSPONDER, {2., 2., 2}, {1., 1., 1.},
                         CTimestamp("115900"));
            ++a;
            ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQ(a.UpdateAge(), 1U);
            ASSERT_FALSE(a.TryUpdate(std::move(a2)));
            ASSERT_EQ(a.UpdateAge(), 1U);
            ASSERT_EQ(a.IdType(), CAircraft::EIdType::RANDOM);
            ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UNKNOWN);
            ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQ(a.Location().Latitude, 1.);
            ASSERT_EQ(a.Location().Longitude, 1.);
            ASSERT_EQ(a.Location().Altitude, 1);
            ASSERT_EQ(a.Movement().ClimbRate, 0.);
            ASSERT_EQ(a.Movement().GndSpeed, 0.);
            ASSERT_EQ(a.Movement().Heading, 0.);
            ASSERT_EQ(a.Timestamp(), CTimestamp("115800"));
        }
        {
            CAircraft a(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::FLARM, {1., 1., 1}, {.0, .0, .0}, CTimestamp("115800"));
            CAircraft a2(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO,
                         CAircraft::ETargetType::FLARM, {2., 2., 2}, {1., 1., 1.}, CTimestamp("115900"));
            ++a;
            ASSERT_EQ(a2.TargetType(), CAircraft::ETargetType::FLARM);
            ASSERT_EQ(a.UpdateAge(), 1U);
            ASSERT_FALSE(a.TryUpdate(std::move(a2)));
            ASSERT_EQ(a.UpdateAge(), 1U);
            ASSERT_EQ(a.IdType(), CAircraft::EIdType::RANDOM);
            ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UNKNOWN);
            ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::FLARM);
            ASSERT_EQ(a.Location().Latitude, 1.);
            ASSERT_EQ(a.Location().Longitude, 1.);
            ASSERT_EQ(a.Location().Altitude, 1);
            ASSERT_EQ(a.Movement().ClimbRate, 0.);
            ASSERT_EQ(a.Movement().GndSpeed, 0.);
            ASSERT_EQ(a.Movement().Heading, 0.);
            ASSERT_EQ(a.Timestamp(), CTimestamp("115800"));
        }
    }
    IT("should refuse update with older timestamp") {
        {
            CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::TRANSPONDER, {1., 1., 1}, {.0, .0, .0}, CTimestamp("115900"));
            CAircraft a2(1, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO,
                         CAircraft::ETargetType::TRANSPONDER, {2., 2., 2}, {1., 1., 1.},
                         CTimestamp("115800"));
            a2.TargetType(CAircraft::ETargetType::TRANSPONDER);
            ++a;
            ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQ(a.UpdateAge(), 1U);
            ASSERT_FALSE(a.TryUpdate(std::move(a2)));
            ASSERT_EQ(a.UpdateAge(), 1U);
            ASSERT_EQ(a.IdType(), CAircraft::EIdType::RANDOM);
            ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UNKNOWN);
            ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::TRANSPONDER);
            ASSERT_EQ(a.Location().Latitude, 1.);
            ASSERT_EQ(a.Location().Longitude, 1.);
            ASSERT_EQ(a.Location().Altitude, 1);
            ASSERT_EQ(a.Movement().ClimbRate, 0.);
            ASSERT_EQ(a.Movement().GndSpeed, 0.);
            ASSERT_EQ(a.Movement().Heading, 0.);
            ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
        }
        {
            CAircraft a(0, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::FLARM, {1., 1., 1}, {.0, .0, .0}, CTimestamp("115900"));
            CAircraft a2(1, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO,
                         CAircraft::ETargetType::FLARM, {2., 2., 2}, {1., 1., 1.}, CTimestamp("115800"));
            ++a;
            ASSERT_EQ(a2.TargetType(), CAircraft::ETargetType::FLARM);
            ASSERT_EQ(a.UpdateAge(), 1U);
            ASSERT_FALSE(a.TryUpdate(std::move(a2)));
            ASSERT_EQ(a.UpdateAge(), 1U);
            ASSERT_EQ(a.IdType(), CAircraft::EIdType::RANDOM);
            ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UNKNOWN);
            ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::FLARM);
            ASSERT_EQ(a.Location().Latitude, 1.);
            ASSERT_EQ(a.Location().Longitude, 1.);
            ASSERT_EQ(a.Location().Altitude, 1);
            ASSERT_EQ(a.Movement().ClimbRate, 0.);
            ASSERT_EQ(a.Movement().GndSpeed, 0.);
            ASSERT_EQ(a.Movement().Heading, 0.);
            ASSERT_EQ(a.Timestamp(), CTimestamp("115900"));
        }
    }
    IT("should refuse update from transponder if is flarm") {
        CAircraft a(0, "#00001", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UFO,
                    CAircraft::ETargetType::FLARM, {2., 2., 2}, {1., 1., 1.}, CTimestamp("115800"));
        CAircraft a2(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                     CAircraft::ETargetType::TRANSPONDER, {1., 1., 1}, {.0, .0, .0}, CTimestamp("115900"));
        ++a;
        ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::FLARM);
        ASSERT_EQ(a2.TargetType(), CAircraft::ETargetType::TRANSPONDER);
        ASSERT_EQ(a.UpdateAge(), 1U);
        ASSERT_FALSE(a.TryUpdate(std::move(a2)));
        ASSERT_EQ(a.UpdateAge(), 1U);
        ASSERT_EQ(a.IdType(), CAircraft::EIdType::ICAO);
        ASSERT_EQ(a.AircraftType(), CAircraft::EAircraftType::UFO);
        ASSERT_EQ(a.TargetType(), CAircraft::ETargetType::FLARM);
        ASSERT_EQ(a.Location().Latitude, 2.);
        ASSERT_EQ(a.Location().Longitude, 2.);
        ASSERT_EQ(a.Location().Altitude, 2);
        ASSERT_EQ(a.Movement().ClimbRate, 1.);
        ASSERT_EQ(a.Movement().GndSpeed, 1.);
        ASSERT_EQ(a.Movement().Heading, 1.);
        ASSERT_EQ(a.Timestamp(), CTimestamp("115800"));
    }
    IT("should throw if given values are outside acceptable range") {
        ASSERT_THROWS(CAircraft(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                                CAircraft::ETargetType::TRANSPONDER, {SLocation::MIN_LATITUDE - 1., 1., 1},
                                {.0, .0, .0}, CTimestamp("115900")),
                      util::error::CBoundsExceededError);
        ASSERT_THROWS(CAircraft(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                                CAircraft::ETargetType::TRANSPONDER, {SLocation::MAX_LATITUDE + 1., 1., 1},
                                {.0, .0, .0}, CTimestamp("115900")),
                      util::error::CBoundsExceededError);
        ASSERT_THROWS(CAircraft(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                                CAircraft::ETargetType::TRANSPONDER, {1., SLocation::MIN_LONGITUDE - 1., 1},
                                {.0, .0, .0}, CTimestamp("115900")),
                      util::error::CBoundsExceededError);
        ASSERT_THROWS(CAircraft(1, "#00001", CAircraft::EIdType::RANDOM, CAircraft::EAircraftType::UNKNOWN,
                                CAircraft::ETargetType::TRANSPONDER, {1., SLocation::MAX_LONGITUDE + 1., 1},
                                {.0, .0, .0}, CTimestamp("115900")),
                      util::error::CBoundsExceededError);
    };
};

DESCRIBE_PAR("test_CTimestamp") {
    SETUP() {
        date_time::Day(1);
        date_time::Now(12, 0, 0);
    }

    IT("should not throw with valid format") {
        ASSERT_NOTHROW(CTimestamp("120000"));
        ASSERT_NOTHROW(CTimestamp("12:00:00.000"));
    }
    IT("should throw with invalid format") {
        ASSERT_THROWS(CTimestamp(""), vfrb::object::error::CTimestampParseError);
        ASSERT_THROWS(CTimestamp("adgjhag"), vfrb::object::error::CTimestampParseError);
        ASSERT_THROWS(CTimestamp("36:60:11.11111"), vfrb::object::error::CTimestampParseError);
        ASSERT_THROWS(CTimestamp("366022"), vfrb::object::error::CTimestampParseError);
    }
    IT("shoudl compare correctly on same day") {
        date_time::Day(1);
        date_time::Now(13, 0, 0);
        CTimestamp const t1("120000");
        CTimestamp const t2("120001");

        ASSERT(t2, GT, t1);
        ASSERT_NOT(t1, GT, t2);
    }
    IT("should compare correctly from old day") {
        date_time::Day(1);
        date_time::Now(23, 59, 0);
        CTimestamp const t1("235800");
        date_time::Day(2);
        date_time::Now(0, 0, 0);
        CTimestamp const t2("235900");

        ASSERT(t2, GT, t1);
        ASSERT_NOT(t1, GT, t2);
    }
    IT("should compare correctly from new day") {
        date_time::Day(1);
        date_time::Now(23, 59, 0);
        CTimestamp const t1("235800");
        date_time::Day(2);
        date_time::Now(0, 1, 0);
        CTimestamp const t2("000000");

        ASSERT(t2, GT, t1);
        ASSERT_NOT(t1, GT, t2);
    }
    IT("should compare correctly from different days") {
        date_time::Day(1);
        date_time::Now(13, 0, 0);
        CTimestamp const t1("120000");
        date_time::Day(2);
        CTimestamp const t2("110000");

        ASSERT(t2, GT, t1);
        ASSERT_NOT(t1, GT, t2);
    };
};
