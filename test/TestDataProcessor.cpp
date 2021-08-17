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

#include "data/processor/CAircraftProcessor.hpp"
#include "data/processor/CGpsProcessor.hpp"
#include "math/Math.hpp"

#include "Helper.hpp"

using namespace vfrb;
using namespace data::processor;
using namespace object;
using namespace str_util;

DESCRIBE("test_CGpsProcessor") {
    CGpsProcessor uut;
    String        nmea;

    AFTER_EACH() {
        nmea.clear();
    }

    IT("should process GPS data correctly") {
        CGpsPosition   pos(0, {0., 0., 0}, 48., 1., 3, 5, CTimestamp("115800"));
        StringInserter ins(nmea);
        uut.Process(pos, ins);
        ASSERT(nmea, LIKE, helper::GpsRE);
    };
};

DESCRIBE("test_CAircraftProcessor") {
    CAircraftProcessor uut;
    String             nmea;

    i32 const M1000 = math::DoubleToInt(math::FEET_2_M * 3281);

    AFTER_EACH() {
        nmea.clear();
    }

    IT("should process an aircraft above, at same position correctly") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::POWERED_AIRCRAFT,
                    CAircraft::ETargetType::TRANSPONDER, {49.0, 8.0, M1000}, {.0, .0, .0}, CTimestamp());
        uut.ReferTo({49.0, 8.0, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "0");
        ASSERT_EQ(match.str(2), "1000");
        ASSERT_EQ(match.str(3), "0");
        ASSERT_EQ(match.str(4), "AAAAAA");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "0");
        ASSERT_EQ(match.str(2), "0");
        ASSERT_EQ(match.str(3), "1000");
        ASSERT_EQ(match.str(5), "AAAAAA");
        ASSERT_EQ(match.str(9), "8");
    }
    IT("should filter an aircraft in distance over threshold") {
        CAircraftProcessor uut(100);
        CAircraft          a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                             CAircraft::ETargetType::TRANSPONDER, {66., 66.0, M1000}, {.0, .0, .0}, CTimestamp());
        uut.ReferTo({49.1, 8.1, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        ASSERT_EQ(nmea.length(), 0U);
        ASSERT_EQ(nmea, "");
    }

    IT("should correctly process when crossing equator from S to N") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {.1, .0, M1000}, {.0, .0, .0}, CTimestamp());
        uut.ReferTo({-0.1, .0, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "0");
        ASSERT_EQ(match.str(3), "22239");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "22239");
        ASSERT_EQ(match.str(2), "0");
        ASSERT_EQ(match.str(3), "1000");
    }
    IT("should correctly process when crossing equator from N to S") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {-0.1, .0, M1000}, {.0, .0, .0}, CTimestamp());
        uut.ReferTo({.1, .0, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "180");
        ASSERT_EQ(match.str(3), "22239");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "-22239");
        ASSERT_EQ(match.str(2), "0");
        ASSERT_EQ(match.str(3), "1000");
    }
    IT("should correctly process when crossing northpole") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {89.9, .0, M1000}, {.0, .0, .0}, CTimestamp());
        uut.ReferTo({89.9, 180., 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "0");
        ASSERT_EQ(match.str(3), "22239");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "22239");
        ASSERT_EQ(match.str(2), "0");
        ASSERT_EQ(match.str(3), "1000");
    }
    IT("should correctly process when crossing southpole") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {-89.9, .0, M1000}, {.0, .0, .0}, CTimestamp());
        uut.ReferTo({-89.9, 180., 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "-180");
        ASSERT_EQ(match.str(3), "22239");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "-22239");
        ASSERT_EQ(match.str(2), "0");
        ASSERT_EQ(match.str(3), "1000");
    }
    IT("should correctly process when crossing 0-Meridian from E to W") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {.0, -0.1, M1000}, {.0, .0, .0}, CTimestamp());
        uut.ReferTo({.0, .1, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "-90");
        ASSERT_EQ(match.str(3), "22239");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "0");
        ASSERT_EQ(match.str(2), "-22239");
        ASSERT_EQ(match.str(3), "1000");
    }
    IT("should correctly process when crossing 0-Meridian from W to E") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {.0, .1, M1000}, {.0, .0, .0}, CTimestamp());
        uut.ReferTo({.0, -0.1, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "90");
        ASSERT_EQ(match.str(3), "22239");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "0");
        ASSERT_EQ(match.str(2), "22239");
        ASSERT_EQ(match.str(3), "1000");
    }
    IT("should correctly process when crossing 0-Meridian at lat=60 from E to W") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {60., -0.1, M1000}, {.0, .0, .0}, CTimestamp());
        uut.ReferTo({60., .1, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "-90");
        ASSERT_EQ(match.str(3), "11119");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "17");
        ASSERT_EQ(match.str(2), "-11119");
        ASSERT_EQ(match.str(3), "1000");
    }
    IT("should correctly process when crossing 0-Meridian at lat=-60 from W to E") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {-60., .1, M1000}, {.0, .0, .0}, CTimestamp());
        uut.ReferTo({-60., -0.1, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "90");
        ASSERT_EQ(match.str(3), "11119");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "-17");
        ASSERT_EQ(match.str(2), "11119");
        ASSERT_EQ(match.str(3), "1000");
    }
    IT("should correctly process when crossing 180-Meridian at equator from E to W") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {.0, -179.9, M1000}, {.0, .0, .0}, CTimestamp());
        uut.ReferTo({.0, 179.9, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "90");
        ASSERT_EQ(match.str(3), "22239");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "0");
        ASSERT_EQ(match.str(2), "22239");
        ASSERT_EQ(match.str(3), "1000");
    }
    IT("should correctly process when crossing 180-Meridian at equator from W to E") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {.0, 179.9, M1000}, {.0, .0, .0}, CTimestamp());
        uut.ReferTo({.0, -179.9, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "-90");
        ASSERT_EQ(match.str(3), "22239");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "0");
        ASSERT_EQ(match.str(2), "-22239");
        ASSERT_EQ(match.str(3), "1000");
    }
    IT("should correctly process somewhere in North America") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {33.825808, -112.219232, M1000}, {.0, .0, .0},
                    CTimestamp());
        uut.ReferTo({33.653124, -112.692253, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "66");
        ASSERT_EQ(match.str(3), "47768");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "19302");
        ASSERT_EQ(match.str(2), "43695");
        ASSERT_EQ(match.str(3), "1000");
    }
    IT("should correctly process somewhere in South America") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {-34.699833, -58.791788, M1000}, {.0, .0, .0},
                    CTimestamp());
        uut.ReferTo({-34.680059, -58.818111, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "132");
        ASSERT_EQ(match.str(3), "3260");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "-2199");
        ASSERT_EQ(match.str(2), "2407");
        ASSERT_EQ(match.str(3), "1000");
    }
    IT("should correctly process somewhere in North Africa") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {5.386705, -5.750365, M1000}, {.0, .0, .0},
                    CTimestamp());
        uut.ReferTo({5.392435, -5.748392, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "-161");
        ASSERT_EQ(match.str(3), "674");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "-638");
        ASSERT_EQ(match.str(2), "-219");
        ASSERT_EQ(match.str(3), "1000");
    }
    IT("should correctly process somewhere in South Africa") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {-23.229517, 15.049683, M1000}, {.0, .0, .0},
                    CTimestamp());
        uut.ReferTo({-26.069244, 15.484389, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "-8");
        ASSERT_EQ(match.str(3), "318804");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "315692");
        ASSERT_EQ(match.str(2), "-44437");
        ASSERT_EQ(match.str(3), "1000");
    }
    IT("should correctly process somewhere in Australia") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {-26.152199, 133.376684, M1000}, {.0, .0, .0},
                    CTimestamp());
        uut.ReferTo({-25.278208, 133.366885, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "179");
        ASSERT_EQ(match.str(3), "97188");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "-97183");
        ASSERT_EQ(match.str(2), "978");
        ASSERT_EQ(match.str(3), "1000");
    }
    IT("should correctly process somewhere in Central Europe") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {49.719445, 9.087646, M1000}, {.0, .0, .0},
                    CTimestamp());
        uut.ReferTo({49.719521, 9.083279, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "92");
        ASSERT_EQ(match.str(3), "314");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "-8");
        ASSERT_EQ(match.str(2), "314");
        ASSERT_EQ(match.str(3), "1000");
    }
    IT("should correctly process somewhere in Asia") {
        CAircraft a(0, "AAAAAA", CAircraft::EIdType::ICAO, CAircraft::EAircraftType::UNKNOWN,
                    CAircraft::ETargetType::TRANSPONDER, {32.896360, 103.855837, M1000}, {.0, .0, .0},
                    CTimestamp());
        uut.ReferTo({65.900837, 101.570680, 0}, 1013.25);
        StringInserter ins(nmea);
        uut.Process(a, ins);
        std::smatch match;
        ASSERT(nmea, LIKE, helper::PflauRE, match);
        ASSERT_EQ(match.str(1), "176");
        ASSERT_EQ(match.str(3), "3673118");
        ASSERT(nmea, LIKE, helper::PflaaRE, match);
        ASSERT_EQ(match.str(1), "-3666184");
        ASSERT_EQ(match.str(2), "225589");
        ASSERT_EQ(match.str(3), "1000");
    };
};
