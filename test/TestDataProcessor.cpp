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

#include "data/processor/AircraftProcessor.h"
#include "data/processor/GpsProcessor.h"
#include "util/math.hpp"

#include "helper.hpp"

using namespace data::processor;
using namespace object;
using namespace sctf;

void test_data_processor(test::TestSuitesRunner& runner)
{
    describe<GpsProcessor>("Process GPS data", runner)->test("process", [] {
        GpsProcessor  gpsp;
        boost::smatch match;
        GpsPosition   pos({0.0, 0.0, 0}, 48.0);
        gpsp.process(pos);
        bool matched = boost::regex_search(pos.get_serialized(), match, helper::gpsRe);
        assertTrue(matched);
    });

    describe<AircraftProcessor>("Process Aircrafts", runner)
        ->test("Aircraft at,above base pos",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({49.0, 8.0, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({49.0, 8.0, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "0");
                   assertEqStr(match.str(2), "1000");
                   assertEqStr(match.str(3), "0");
                   assertEqStr(match.str(4), "BBBBBB");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "0");
                   assertEqStr(match.str(2), "0");
                   assertEqStr(match.str(3), "1000");
                   assertEqStr(match.str(5), "BBBBBB");
                   assertEqStr(match.str(9), "8");
               })
        ->test("filter distance", [] {
            AircraftProcessor proc(0);
            Aircraft          ac;
            ac.set_id("BBBBBB");
            ac.set_fullInfo(false);
            ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
            ac.set_position({49.0, 8.0, math::doubleToInt(math::FEET_2_M * 3281)});
            proc.referTo({49.1, 8.1, 0}, 1013.25);
            proc.process(ac);
            assertEqStr(ac.get_serialized(), "");
        });

    describeParallel<AircraftProcessor>("process relative positions", runner)
        ->test("Cross Equator S to N",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({0.1, 0.0, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({-0.1, 0.0, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "0");
                   assertEqStr(match.str(3), "22239");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "22239");
                   assertEqStr(match.str(2), "0");
                   assertEqStr(match.str(3), "1000");
               })
        ->test("Cross Equator N to S",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({-0.1, 0.0, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({0.1, 0.0, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "180");
                   assertEqStr(match.str(3), "22239");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "-22239");
                   assertEqStr(match.str(2), "0");
                   assertEqStr(match.str(3), "1000");
               })
        ->test("Cross Northpole",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({89.9, 0.0, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({89.9, 180.0, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "0");
                   assertEqStr(match.str(3), "22239");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "22239");
                   assertEqStr(match.str(2), "0");
                   assertEqStr(match.str(3), "1000");
               })
        ->test("Cross Southpole",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({-89.9, 0.0, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({-89.9, 180.0, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "-180");
                   assertEqStr(match.str(3), "22239");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "-22239");
                   assertEqStr(match.str(2), "0");
                   assertEqStr(match.str(3), "1000");
               })
        ->test("Cross 0-Meridian on Equator E to W",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({0.0, -0.1, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({0.0, 0.1, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "-90");
                   assertEqStr(match.str(3), "22239");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "0");
                   assertEqStr(match.str(2), "-22239");
                   assertEqStr(match.str(3), "1000");
               })
        ->test("Cross 0-Meridian on Equator W to E",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({0.0, 0.1, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({0.0, -0.1, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "90");
                   assertEqStr(match.str(3), "22239");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "0");
                   assertEqStr(match.str(2), "22239");
                   assertEqStr(match.str(3), "1000");
               })
        ->test("Cross 0-Meridian on LAT(60) E to W",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({60.0, -0.1, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({60.0, 0.1, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "-90");
                   assertEqStr(match.str(3), "11119");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "17");
                   assertEqStr(match.str(2), "-11119");
                   assertEqStr(match.str(3), "1000");
               })
        ->test("Cross 0-Meridian on LAT(-60) W to E",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({-60.0, 0.1, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({-60.0, -0.1, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "90");
                   assertEqStr(match.str(3), "11119");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "-17");
                   assertEqStr(match.str(2), "11119");
                   assertEqStr(match.str(3), "1000");
               })
        ->test("Cross 180-Meridian on Equator E to W",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({0.0, -179.9, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({0.0, 179.9, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "90");
                   assertEqStr(match.str(3), "22239");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "0");
                   assertEqStr(match.str(2), "22239");
                   assertEqStr(match.str(3), "1000");
               })
        ->test("Cross 180-Meridian on Equator W to E",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({0.0, 179.9, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({0.0, -179.9, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "-90");
                   assertEqStr(match.str(3), "22239");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "0");
                   assertEqStr(match.str(2), "-22239");
                   assertEqStr(match.str(3), "1000");
               })
        ->test("North America",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({33.825808, -112.219232, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({33.653124, -112.692253, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "66");
                   assertEqStr(match.str(3), "47768");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "19302");
                   assertEqStr(match.str(2), "43695");
                   assertEqStr(match.str(3), "1000");
               })
        ->test("South America",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({-34.699833, -58.791788, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({-34.680059, -58.818111, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "132");
                   assertEqStr(match.str(3), "3260");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "-2199");
                   assertEqStr(match.str(2), "2407");
                   assertEqStr(match.str(3), "1000");
               })
        ->test("North Africa",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({5.386705, -5.750365, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({5.392435, -5.748392, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "-161");
                   assertEqStr(match.str(3), "674");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "-638");
                   assertEqStr(match.str(2), "-219");
                   assertEqStr(match.str(3), "1000");
               })
        ->test("South Africa",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({-23.229517, 15.049683, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({-26.069244, 15.484389, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "-8");
                   assertEqStr(match.str(3), "318804");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "315692");
                   assertEqStr(match.str(2), "-44437");
                   assertEqStr(match.str(3), "1000");
               })
        ->test("Australia",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({-26.152199, 133.376684, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({-25.278208, 133.366885, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "179");
                   assertEqStr(match.str(3), "97188");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "-97183");
                   assertEqStr(match.str(2), "978");
                   assertEqStr(match.str(3), "1000");
               })
        ->test("Central Europe",
               [] {
                   AircraftProcessor proc;
                   Aircraft          ac;
                   ac.set_id("BBBBBB");
                   ac.set_fullInfo(false);
                   ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
                   ac.set_position({49.719445, 9.087646, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.referTo({49.719521, 9.083279, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "92");
                   assertEqStr(match.str(3), "314");
                   matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
                   assertTrue(matched);
                   assertEqStr(match.str(1), "-8");
                   assertEqStr(match.str(2), "314");
                   assertEqStr(match.str(3), "1000");
               })
        ->test("Asia", [] {
            AircraftProcessor proc;
            Aircraft          ac;
            ac.set_id("BBBBBB");
            ac.set_fullInfo(false);
            ac.set_targetType(Aircraft::TargetType::TRANSPONDER);
            ac.set_position({32.896360, 103.855837, math::doubleToInt(math::FEET_2_M * 3281)});
            proc.referTo({65.900837, 101.570680, 0}, 1013.25);
            proc.process(ac);
            boost::smatch match;
            bool          matched = boost::regex_search(ac.get_serialized(), match, helper::pflauRe);
            assertTrue(matched);
            assertEqStr(match.str(1), "176");
            assertEqStr(match.str(3), "3673118");
            matched = boost::regex_search(ac.get_serialized(), match, helper::pflaaRe);
            assertTrue(matched);
            assertEqStr(match.str(1), "-3666184");
            assertEqStr(match.str(2), "225589");
            assertEqStr(match.str(3), "1000");
        });
}
