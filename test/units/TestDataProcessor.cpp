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

#include "../../src/data/processor/AircraftProcessor.h"
#include "../../src/data/processor/GpsProcessor.h"
#include "../Helper.hpp"
#include "../framework/src/framework.h"

#ifdef assert
#undef assert
#endif

using namespace data::processor;
using namespace object;
using namespace testsuite;
using namespace comparator;

void test_data_processor(TestSuitesRunner& runner)
{
    describe<GpsProcessor>("Process GPS data", runner)->test("process", []() {
        GpsProcessor gpsp;
        boost::smatch match;
        GpsPosition pos({0.0, 0.0, 0}, 48.0);
        gpsp.process(pos);
        bool matched = boost::regex_search(pos.getSerialized(), match, helper::gpsRe);
        assert(matched, true, helper::equalsBool);
    });

    describe<AircraftProcessor>("Process Aircrafts", runner)
        ->test("Aircraft at,above base pos",
               []() {
                   AircraftProcessor proc;
                   Aircraft ac;
                   ac.setId("BBBBBB");
                   ac.setFullInfoAvailable(false);
                   ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                   ac.setPosition({49.0, 8.0, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.setRefered({49.0, 8.0, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("0"), helper::equalsStr);
                   assert(match.str(2), std::string("1000"), helper::equalsStr);
                   assert(match.str(3), std::string("0"), helper::equalsStr);
                   assert(match.str(4), std::string("BBBBBB"), helper::equalsStr);
                   matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("0"), helper::equalsStr);
                   assert(match.str(2), std::string("0"), helper::equalsStr);
                   assert(match.str(3), std::string("1000"), helper::equalsStr);
                   assert(match.str(5), std::string("BBBBBB"), helper::equalsStr);
                   assert(match.str(9), std::string("8"), helper::equalsStr);
               })
        ->test("filter distance", []() {
            AircraftProcessor proc(0);
            Aircraft ac;
            ac.setId("BBBBBB");
            ac.setFullInfoAvailable(false);
            ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
            ac.setPosition({49.0, 8.0, math::doubleToInt(math::FEET_2_M * 3281)});
            proc.setRefered({49.1, 8.1, 0}, 1013.25);
            proc.process(ac);
            assert(ac.getSerialized(), std::string(""), helper::equalsStr);
        });

    describeParallel<AircraftProcessor>("process relative positions", runner)
        ->test("Cross Equator S to N",
               []() {
                   AircraftProcessor proc;
                   Aircraft ac;
                   ac.setId("BBBBBB");
                   ac.setFullInfoAvailable(false);
                   ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                   ac.setPosition({0.1, 0.0, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.setRefered({-0.1, 0.0, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("0"), helper::equalsStr);
                   assert(match.str(3), std::string("22239"), helper::equalsStr);
                   matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("22239"), helper::equalsStr);
                   assert(match.str(2), std::string("0"), helper::equalsStr);
                   assert(match.str(3), std::string("1000"), helper::equalsStr);
               })
        ->test("Cross Equator N to S",
               []() {
                   AircraftProcessor proc;
                   Aircraft ac;
                   ac.setId("BBBBBB");
                   ac.setFullInfoAvailable(false);
                   ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                   ac.setPosition({-0.1, 0.0, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.setRefered({0.1, 0.0, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("180"), helper::equalsStr);
                   assert(match.str(3), std::string("22239"), helper::equalsStr);
                   matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("-22239"), helper::equalsStr);
                   assert(match.str(2), std::string("0"), helper::equalsStr);
                   assert(match.str(3), std::string("1000"), helper::equalsStr);
               })
        ->test("Cross Northpole",
               []() {
                   AircraftProcessor proc;
                   Aircraft ac;
                   ac.setId("BBBBBB");
                   ac.setFullInfoAvailable(false);
                   ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                   ac.setPosition({89.9, 0.0, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.setRefered({89.9, 180.0, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("0"), helper::equalsStr);
                   assert(match.str(3), std::string("22239"), helper::equalsStr);
                   matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("22239"), helper::equalsStr);
                   assert(match.str(2), std::string("0"), helper::equalsStr);
                   assert(match.str(3), std::string("1000"), helper::equalsStr);
               })
        ->test("Cross Southpole",
               []() {
                   AircraftProcessor proc;
                   Aircraft ac;
                   ac.setId("BBBBBB");
                   ac.setFullInfoAvailable(false);
                   ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                   ac.setPosition({-89.9, 0.0, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.setRefered({-89.9, 180.0, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("-180"), helper::equalsStr);
                   assert(match.str(3), std::string("22239"), helper::equalsStr);
                   matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("-22239"), helper::equalsStr);
                   assert(match.str(2), std::string("0"), helper::equalsStr);
                   assert(match.str(3), std::string("1000"), helper::equalsStr);
               })
        ->test("Cross 0-Meridian on Equator E to W",
               []() {
                   AircraftProcessor proc;
                   Aircraft ac;
                   ac.setId("BBBBBB");
                   ac.setFullInfoAvailable(false);
                   ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                   ac.setPosition({0.0, -0.1, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.setRefered({0.0, 0.1, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("-90"), helper::equalsStr);
                   assert(match.str(3), std::string("22239"), helper::equalsStr);
                   matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("0"), helper::equalsStr);
                   assert(match.str(2), std::string("-22239"), helper::equalsStr);
                   assert(match.str(3), std::string("1000"), helper::equalsStr);
               })
        ->test("Cross 0-Meridian on Equator W to E",
               []() {
                   AircraftProcessor proc;
                   Aircraft ac;
                   ac.setId("BBBBBB");
                   ac.setFullInfoAvailable(false);
                   ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                   ac.setPosition({0.0, 0.1, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.setRefered({0.0, -0.1, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("90"), helper::equalsStr);
                   assert(match.str(3), std::string("22239"), helper::equalsStr);
                   matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("0"), helper::equalsStr);
                   assert(match.str(2), std::string("22239"), helper::equalsStr);
                   assert(match.str(3), std::string("1000"), helper::equalsStr);
               })
        ->test("Cross 0-Meridian on LAT(60) E to W",
               []() {
                   AircraftProcessor proc;
                   Aircraft ac;
                   ac.setId("BBBBBB");
                   ac.setFullInfoAvailable(false);
                   ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                   ac.setPosition({60.0, -0.1, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.setRefered({60.0, 0.1, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("-90"), helper::equalsStr);
                   assert(match.str(3), std::string("11119"), helper::equalsStr);
                   matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("17"), helper::equalsStr);
                   assert(match.str(2), std::string("-11119"), helper::equalsStr);
                   assert(match.str(3), std::string("1000"), helper::equalsStr);
               })
        ->test("Cross 0-Meridian on LAT(-60) W to E",
               []() {
                   AircraftProcessor proc;
                   Aircraft ac;
                   ac.setId("BBBBBB");
                   ac.setFullInfoAvailable(false);
                   ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                   ac.setPosition({-60.0, 0.1, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.setRefered({-60.0, -0.1, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("90"), helper::equalsStr);
                   assert(match.str(3), std::string("11119"), helper::equalsStr);
                   matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("-17"), helper::equalsStr);
                   assert(match.str(2), std::string("11119"), helper::equalsStr);
                   assert(match.str(3), std::string("1000"), helper::equalsStr);
               })
        ->test(
            "Cross 180-Meridian on Equator E to W",
            []() {
                AircraftProcessor proc;
                Aircraft ac;
                ac.setId("BBBBBB");
                ac.setFullInfoAvailable(false);
                ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                ac.setPosition({0.0, -179.9, math::doubleToInt(math::FEET_2_M * 3281)});
                proc.setRefered({0.0, 179.9, 0}, 1013.25);
                proc.process(ac);
                boost::smatch match;
                bool matched
                    = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                assert(matched, true, helper::equalsBool);
                assert(match.str(1), std::string("90"), helper::equalsStr);
                assert(match.str(3), std::string("22239"), helper::equalsStr);
                matched = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                assert(matched, true, helper::equalsBool);
                assert(match.str(1), std::string("0"), helper::equalsStr);
                assert(match.str(2), std::string("22239"), helper::equalsStr);
                assert(match.str(3), std::string("1000"), helper::equalsStr);
            })
        ->test("Cross 180-Meridian on Equator W to E",
               []() {
                   AircraftProcessor proc;
                   Aircraft ac;
                   ac.setId("BBBBBB");
                   ac.setFullInfoAvailable(false);
                   ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                   ac.setPosition({0.0, 179.9, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.setRefered({0.0, -179.9, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("-90"), helper::equalsStr);
                   assert(match.str(3), std::string("22239"), helper::equalsStr);
                   matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("0"), helper::equalsStr);
                   assert(match.str(2), std::string("-22239"), helper::equalsStr);
                   assert(match.str(3), std::string("1000"), helper::equalsStr);
               })
        ->test("North America",
               []() {
                   AircraftProcessor proc;
                   Aircraft ac;
                   ac.setId("BBBBBB");
                   ac.setFullInfoAvailable(false);
                   ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                   ac.setPosition({33.825808, -112.219232,
                                   math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.setRefered({33.653124, -112.692253, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("66"), helper::equalsStr);
                   assert(match.str(3), std::string("47768"), helper::equalsStr);
                   matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("19302"), helper::equalsStr);
                   assert(match.str(2), std::string("43695"), helper::equalsStr);
                   assert(match.str(3), std::string("1000"), helper::equalsStr);
               })
        ->test("South America",
               []() {
                   AircraftProcessor proc;
                   Aircraft ac;
                   ac.setId("BBBBBB");
                   ac.setFullInfoAvailable(false);
                   ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                   ac.setPosition({-34.699833, -58.791788,
                                   math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.setRefered({-34.680059, -58.818111, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("132"), helper::equalsStr);
                   assert(match.str(3), std::string("3260"), helper::equalsStr);
                   matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("-2199"), helper::equalsStr);
                   assert(match.str(2), std::string("2407"), helper::equalsStr);
                   assert(match.str(3), std::string("1000"), helper::equalsStr);
               })
        ->test("North Africa",
               []() {
                   AircraftProcessor proc;
                   Aircraft ac;
                   ac.setId("BBBBBB");
                   ac.setFullInfoAvailable(false);
                   ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                   ac.setPosition(
                       {5.386705, -5.750365, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.setRefered({5.392435, -5.748392, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("-161"), helper::equalsStr);
                   assert(match.str(3), std::string("674"), helper::equalsStr);
                   matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("-638"), helper::equalsStr);
                   assert(match.str(2), std::string("-219"), helper::equalsStr);
                   assert(match.str(3), std::string("1000"), helper::equalsStr);
               })
        ->test("South Africa",
               []() {
                   AircraftProcessor proc;
                   Aircraft ac;
                   ac.setId("BBBBBB");
                   ac.setFullInfoAvailable(false);
                   ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                   ac.setPosition(
                       {-23.229517, 15.049683, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.setRefered({-26.069244, 15.484389, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("-8"), helper::equalsStr);
                   assert(match.str(3), std::string("318804"), helper::equalsStr);
                   matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("315692"), helper::equalsStr);
                   assert(match.str(2), std::string("-44437"), helper::equalsStr);
                   assert(match.str(3), std::string("1000"), helper::equalsStr);
               })
        ->test("Australia",
               []() {
                   AircraftProcessor proc;
                   Aircraft ac;
                   ac.setId("BBBBBB");
                   ac.setFullInfoAvailable(false);
                   ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                   ac.setPosition({-26.152199, 133.376684,
                                   math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.setRefered({-25.278208, 133.366885, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("179"), helper::equalsStr);
                   assert(match.str(3), std::string("97188"), helper::equalsStr);
                   matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("-97183"), helper::equalsStr);
                   assert(match.str(2), std::string("978"), helper::equalsStr);
                   assert(match.str(3), std::string("1000"), helper::equalsStr);
               })
        ->test("Central Europe",
               []() {
                   AircraftProcessor proc;
                   Aircraft ac;
                   ac.setId("BBBBBB");
                   ac.setFullInfoAvailable(false);
                   ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
                   ac.setPosition(
                       {49.719445, 9.087646, math::doubleToInt(math::FEET_2_M * 3281)});
                   proc.setRefered({49.719521, 9.083279, 0}, 1013.25);
                   proc.process(ac);
                   boost::smatch match;
                   bool matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("92"), helper::equalsStr);
                   assert(match.str(3), std::string("314"), helper::equalsStr);
                   matched
                       = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
                   assert(matched, true, helper::equalsBool);
                   assert(match.str(1), std::string("-8"), helper::equalsStr);
                   assert(match.str(2), std::string("314"), helper::equalsStr);
                   assert(match.str(3), std::string("1000"), helper::equalsStr);
               })
        ->test("Asia", []() {
            AircraftProcessor proc;
            Aircraft ac;
            ac.setId("BBBBBB");
            ac.setFullInfoAvailable(false);
            ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
            ac.setPosition(
                {32.896360, 103.855837, math::doubleToInt(math::FEET_2_M * 3281)});
            proc.setRefered({65.900837, 101.570680, 0}, 1013.25);
            proc.process(ac);
            boost::smatch match;
            bool matched
                = boost::regex_search(ac.getSerialized(), match, helper::pflauRe);
            assert(matched, true, helper::equalsBool);
            assert(match.str(1), std::string("176"), helper::equalsStr);
            assert(match.str(3), std::string("3673118"), helper::equalsStr);
            matched = boost::regex_search(ac.getSerialized(), match, helper::pflaaRe);
            assert(matched, true, helper::equalsBool);
            assert(match.str(1), std::string("-3666184"), helper::equalsStr);
            assert(match.str(2), std::string("225589"), helper::equalsStr);
            assert(match.str(3), std::string("1000"), helper::equalsStr);
        });
}
