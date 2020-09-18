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

#include <string>

#include "data/CAircraftData.hpp"
#include "data/CAtmosphereData.hpp"
#include "data/CGpsData.hpp"
#include "data/CWindData.hpp"
#include "feed/parser/CAprsParser.hpp"
#include "feed/parser/CSbsParser.hpp"
#include "math/Math.hpp"

#include "Helper.hpp"

using sctf::LIKE;
using sctf::FEQ;

using namespace vfrb;
using namespace data;
using namespace object;

DESCRIBE("test_CAircraftData") {
    SPtr<CAircraftData> uut = std::make_shared<CAircraftData>([this](SAccessor const& a_) {
        nmea = a_.Nmea;
        ac   = &static_cast<CAircraft const&>(a_.Obj);
    });
    StringView          nmea;
    CAircraft const*    ac    = nullptr;
    s32 const           M1000 = math::DoubleToInt(math::FEET_2_M * 3281);

    SETUP() {
        uut->Environment({49., 8., 0}, 1013.25);
    }

    AFTER_EACH() {
        nmea = StringView();
        ac   = nullptr;
    }

    IT("should report an aircraft on access") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {49., 8., M1000}, CTimestamp());
        ASSERT_TRUE(uut->Update(std::move(a)));
        uut->Access();
        ASSERT_FALSE(nmea.empty());
        ASSERT_NOT_NULL(ac);
        ASSERT(nmea, LIKE(), helper::PflauRE);
        ASSERT(nmea, LIKE(), helper::PflaaRE);
    }
    IT("should not report an aircraft after outdate-interval, and set it as transponder target") {
        ASSERT_EQ(CObject::OUTDATED, CAircraftData::NO_FLARM_THRESHOLD);
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::FLARM, {49., 8., M1000}, CTimestamp());
        ASSERT_TRUE(uut->Update(std::move(a)));
        for (auto i = 0; i < CAircraftData::NO_FLARM_THRESHOLD; ++i) {
            nmea = StringView();
            uut->Access();
        }
        ASSERT_TRUE(nmea.empty());
        ASSERT_NOT_NULL(ac);
        ASSERT_EQ(ac->TargetType(), CAircraft::ETargetType::TRANSPONDER);
    }
    IT("should delete an aircraft after certain interval") {
        ASSERT_EQ(uut->Size(), 1);
        for (auto i = 0; i < CAircraftData::DELETE_THRESHOLD; ++i) {
            uut->Access();
        }
        ASSERT_EQ(uut->Size(), 0);
    }
    IT("should prefer FLARM, and accept TRANSPONDER again after certain interval") {
        {
            CAircraft a(1, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::TRANSPONDER, {49., 8., M1000}, CTimestamp());
            ASSERT_TRUE(uut->Update(std::move(a)));
        }
        {
            CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::FLARM, {49., 8., M1000}, CTimestamp());
            ASSERT_TRUE(uut->Update(std::move(a)));
        }
        {
            CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::TRANSPONDER, {49., 8., M1000}, CTimestamp());
            ASSERT_FALSE(uut->Update(std::move(a)));
        }
        for (auto i = 0; i < CAircraftData::NO_FLARM_THRESHOLD; ++i) {
            uut->Access();
        }
        {
            CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                        CAircraft::ETargetType::TRANSPONDER, {49., 8., M1000}, CTimestamp());
            ASSERT_TRUE(uut->Update(std::move(a)));
        }
    };
};

DESCRIBE("test_CGpsData") {
    SPtr<CGpsData> uut = std::make_shared<CGpsData>(
        [this](SAccessor const& a_) {
            nmea = a_.Nmea;
            pos  = &static_cast<CGpsPosition const&>(a_.Obj);
        },
        CGpsPosition{0, {0., 0., 0}, 0.}, true);
    StringView          nmea;
    CGpsPosition const* pos = nullptr;

    AFTER_EACH() {
        nmea = StringView();
        pos  = nullptr;
    }

    IT("should report a gps position on access") {
        {
            CGpsPosition p(0, {10., 85., 100}, 40.);
            ASSERT_TRUE(uut->Update(std::move(p)));
        }
        uut->Access();
        auto p = uut->Location();
        ASSERT_EQ(p.Latitude, 10.);
        ASSERT_EQ(p.Longitude, 85.);
        ASSERT_EQ(p.Altitude, 100);
        ASSERT_FALSE(nmea.empty());
        ASSERT_NOT_NULL(pos);
        ASSERT(nmea, LIKE(), helper::GpsRE);
    }
    IT("should throw if received position is good") {
        CGpsPosition p(0, {10., 85., 100}, 40., 2., 7, 1, CTimestamp());
        ASSERT_THROWS(uut->Update(std::move(p)), vfrb::data::error::CReceivedGoodPosition);
    }
    IT("should throw if position was already locked") {
        CGpsPosition p(0, {10., 85., 100}, 40.);
        ASSERT_THROWS(uut->Update(std::move(p)), vfrb::data::error::CPositionAlreadyLocked);
    };
};

DESCRIBE("test_CWindData") {
    SPtr<CWindData> uut = std::make_shared<CWindData>([this](SAccessor const& a_) {
        nmea = a_.Nmea;
        wind = &static_cast<CWind const&>(a_.Obj);
    });
    StringView      nmea;
    CWind const*    wind;

    AFTER_EACH() {
        nmea = StringView();
        wind = nullptr;
    }

    IT("should report the wind on access") {
        CWind w(0, "$WIMWV,242.8,R,6.9,N,A*2");
        ASSERT_TRUE(uut->Update(std::move(w)));
        uut->Access();
        ASSERT_FALSE(nmea.empty());
        ASSERT(nmea, LIKE(), "$WIMWV,242.8,R,6.9,N,A*2");
        ASSERT_NOT_NULL(wind);
    }
    IT("should clear wind after access without update") {
        uut->Access();
        ASSERT_EQ(uut->Size(), 0);
        ASSERT_TRUE(nmea.empty());
    };
};

DESCRIBE("test_CAtmosphereData") {
    SPtr<CAtmosphereData> uut = std::make_shared<CAtmosphereData>(
        [this](SAccessor const& a_) {
            nmea = a_.Nmea;
            atm  = &static_cast<CAtmosphere const&>(a_.Obj);
        },
        CAtmosphere{0, "$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E"});
    StringView         nmea;
    CAtmosphere const* atm;

    AFTER_EACH() {
        nmea = StringView();
        atm  = nullptr;
    }

    IT("should report the atmosphere on access") {
        CAtmosphere a(0, "$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E");
        ASSERT_TRUE(uut->Update(std::move(a)));
        uut->Access();
        ASSERT_FALSE(nmea.empty());
        ASSERT(nmea, LIKE(), "$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E");
        ASSERT_NOT_NULL(atm);
        ASSERT(atm->Pressure(), FEQ(), 1009.1);
    }
};
