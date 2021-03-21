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

#define VFRB_TEST_MODE

#include "data/CAircraftData.hpp"
#include "data/CAtmosphereData.hpp"
#include "data/CGpsData.hpp"
#include "data/CWindData.hpp"
#include "feed/parser/CAprsParser.hpp"
#include "feed/parser/CSbsParser.hpp"
#include "math/Math.hpp"

#include "Helper.hpp"

using namespace vfrb;
using namespace data;
using namespace object;
using namespace str_util;

DESCRIBE("test_CAircraftData") {
    Shared<CAircraftData> uut;
    String                nmea;
    i32 const             M1000 = math::DoubleToInt(math::FEET_2_M * 3281);

    BEFORE_EACH() {
        uut = AllocShared<CAircraftData>();
        uut->Environment({49., 8., 0}, 1013.25);
    }

    AFTER_EACH() {
        nmea.clear();
    }

    IT("should report an aircraft on access") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {49., 8., M1000}, CTimestamp());
        ASSERT_TRUE(uut->Update(std::move(a)));
        uut->CollectInto(StringInserter(&nmea));
        ASSERT_FALSE(nmea.empty());
        ASSERT(nmea, LIKE, helper::PflauRE);
        ASSERT(nmea, LIKE, helper::PflaaRE);
    }
    IT("should not report an aircraft after outdate-interval, and set it as transponder target") {
        ASSERT_EQ(CObject::OUTDATED, CAircraftData::NO_FLARM_THRESHOLD);
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::FLARM, {49., 8., M1000}, CTimestamp());
        ASSERT_TRUE(uut->Update(std::move(a)));
        for (auto i = 0U; i <= CAircraftData::NO_FLARM_THRESHOLD; ++i) {
            nmea.clear();
            uut->CollectInto(StringInserter(&nmea));
        }
        ASSERT_TRUE(nmea.empty());
        ASSERT_EQ(uut->Container().Begin()->Value.TargetType(), CAircraft::ETargetType::TRANSPONDER);
    }
    IT("should delete an aircraft after certain interval") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::FLARM, {49., 8., M1000}, CTimestamp());
        ASSERT_TRUE(uut->Update(std::move(a)));
        ASSERT_EQ(uut->Size(), 1U);
        for (auto i = 0; i < CAircraftData::DELETE_THRESHOLD; ++i) {
            uut->CollectInto(StringInserter(&nmea));
        }
        ASSERT_EQ(uut->Size(), 0U);
    }
    IT("should prefer FLARM, and accept TRANSPONDER again after certain interval") {
        {
            CAircraft a(1, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::TRANSPONDER, {49., 8., M1000}, CTimestamp("120000"));
            ASSERT_TRUE(uut->Update(std::move(a)));
        }
        {
            CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::FLARM, {49., 8., M1000}, CTimestamp("120001"));
            ASSERT_TRUE(uut->Update(std::move(a)));
        }
        {
            CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::TRANSPONDER, {49., 8., M1000}, CTimestamp("120002"));
            ASSERT_FALSE(uut->Update(std::move(a)));
        }
        for (auto i = 0U; i < CAircraftData::NO_FLARM_THRESHOLD; ++i) {
            uut->CollectInto(StringInserter(&nmea));
        }
        {
            CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::TRANSPONDER, {49., 8., M1000}, CTimestamp("120003"));
            ASSERT_TRUE(uut->Update(std::move(a)));
        }
    };
};

DESCRIBE("test_CGpsData") {
    Shared<CGpsData> uut;
    String           nmea;

    BEFORE_EACH() {
        uut = AllocShared<CGpsData>(

            CGpsPosition{0, {0., 0., 0}, 0.}, true);
    }

    AFTER_EACH() {
        nmea.clear();
    }

    IT("should report a gps position on access") {
        {
            CGpsPosition p(0, {10., 85., 100}, 40., .0, 5, 1, CTimestamp("120000"));
            ASSERT_TRUE(uut->Update(std::move(p)));
        }
        uut->CollectInto(StringInserter(&nmea));
        auto p = uut->Location();
        ASSERT_EQ(p.Latitude, 10.);
        ASSERT_EQ(p.Longitude, 85.);
        ASSERT_EQ(p.Altitude, 100);
        ASSERT_FALSE(nmea.empty());
        ASSERT(nmea, LIKE, helper::GpsRE);
    }
    IT("should throw if received position is good") {
        CGpsPosition p(0, {10., 85., 100}, 40., 2., 7, 1, CTimestamp("120000"));
        ASSERT_THROWS(uut->Update(std::move(p)), vfrb::data::error::CReceivedGoodPosition);
    }
    IT("should throw if position was already locked") {
        {
            CGpsPosition p(0, {10., 85., 100}, 40., 2., 7, 1, CTimestamp("120000"));
            ASSERT_THROWS(uut->Update(std::move(p)), vfrb::data::error::CReceivedGoodPosition);
        }
        CGpsPosition p(0, {10., 85., 100}, 40., 2., 7, 1, CTimestamp("120001"));
        ASSERT_THROWS(uut->Update(std::move(p)), vfrb::data::error::CPositionAlreadyLocked);
    };
};

DESCRIBE("test_CWindData") {
    Shared<CWindData> uut;
    String            nmea;

    BEFORE_EACH() {
        uut = AllocShared<CWindData>();
    }

    AFTER_EACH() {
        nmea.clear();
    }

    IT("should report the wind on access") {
        CWind w(0, "$WIMWV,242.8,R,6.9,N,A*2");
        ASSERT_TRUE(uut->Update(std::move(w)));
        uut->CollectInto(StringInserter(&nmea));
        ASSERT_FALSE(nmea.empty());
        ASSERT_EQ(nmea, "$WIMWV,242.8,R,6.9,N,A*2");
    }
    IT("should clear wind after access without update") {
        CWind w(0, "$WIMWV,242.8,R,6.9,N,A*2");
        ASSERT_TRUE(uut->Update(std::move(w)));
        uut->CollectInto(StringInserter(&nmea));
        nmea.clear();
        uut->CollectInto(StringInserter(&nmea));
        ASSERT_EQ(uut->Size(), 0U);
        ASSERT_TRUE(nmea.empty());
    };
};

DESCRIBE("test_CAtmosphereData") {
    Shared<CAtmosphereData> uut;
    String                  nmea;

    BEFORE_EACH() {
        uut =
            AllocShared<CAtmosphereData>(CAtmosphere{0, "$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E"});
    }

    AFTER_EACH() {
        nmea.clear();
    }

    IT("should report the atmosphere on access") {
        CAtmosphere a(0, 1009.1, "$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E");
        ASSERT_TRUE(uut->Update(std::move(a)));
        uut->CollectInto(StringInserter(&nmea));
        ASSERT_FALSE(nmea.empty());
        ASSERT_EQ(nmea, "$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E");
        ASSERT(uut->Pressure(), EQ, 1009.1);
    }
};
