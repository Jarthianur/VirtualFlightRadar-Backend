/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

#include <cstdint>
#include <string>
#include <boost/regex.hpp>

#include "../../src/aircraft/AircraftProcessor.h"
#include "../../src/config/Configuration.h"
#include "../../src/data/AircraftContainer.h"
#include "../../src/util/Priority.h"
#include "../../src/vfrb/VFRB.h"
#include "../framework/src/testsuite/TestSuite.hpp"
#include "../framework/src/testsuite/TestSuitesRunner.hpp"
#include "../framework/src/util/assert.hpp"
#include "../Helper.hpp"

#ifdef assert
#undef assert
#endif

using namespace util;
using namespace testsuite;
using namespace comparator;

void test_aircraft(TestSuitesRunner& runner)
{
    describe<aircraft::AircraftProcessor>("Process Aircrafts", runner)->test(
            "setup", []()
            {
                helper::clearAcCont();
                config::Configuration::base_altitude = 0;
                config::Configuration::base_latitude = 49.000000;
                config::Configuration::base_longitude = 8.000000;
                config::Configuration::base_pressure = 1013.25;
                helper::setupVFRB();
            })->test("Aircraft at,above base pos",
            []()
            {
                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.000000,8.000000,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);

                assert(match.str(1), std::string("0"), helper::eqs);
                assert(match.str(2), std::string("1000"), helper::eqs);
                assert(match.str(3), std::string("0"), helper::eqs);
                assert(match.str(4), std::string("BBBBBB"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);

                assert(match.str(1), std::string("0"), helper::eqs);
                assert(match.str(2), std::string("0"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
                assert(match.str(5), std::string("BBBBBB"), helper::eqs);
                assert(match.str(9), std::string("8"), helper::eqs);
            })->test("filter distance",
            []()
            {
                config::Configuration::filter_maxDist = 10000;
                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.100000,8.100000,,,,,,0", Priority::DONTCARE);
                assert(vfrb::VFRB::msAcCont.processAircrafts(), std::string(""), helper::eqs);
                config::Configuration::filter_maxDist = INT32_MAX;
            });

    describe<aircraft::AircraftProcessor>("process relative positions", runner)->test(
            "setup", []()
            {
                helper::clearAcCont();
                config::Configuration::base_altitude = 0;
                config::Configuration::base_latitude = -0.100000;
                config::Configuration::base_longitude = 0.000000;
                config::Configuration::base_pressure = 1013.25;
                helper::setupVFRB();
            })->test("Cross Equator S to N",
            []()
            {
                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,0.100000,0.000000,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("0"), helper::eqs);
                assert(match.str(3), std::string("22239"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("22239"), helper::eqs);
                assert(match.str(2), std::string("0"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            })->test("Cross Equator N to S",
            []()
            {
                config::Configuration::base_latitude = 0.100000;
                config::Configuration::base_longitude = 0.000000;
                helper::setupVFRB();

                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,-0.100000,0.000000,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("180"), helper::eqs);
                assert(match.str(3), std::string("22239"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("-22239"), helper::eqs);
                assert(match.str(2), std::string("0"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            })->test("Cross Northpole",
            []()
            {
                config::Configuration::base_latitude = 89.900000;
                config::Configuration::base_longitude = 180.000000;
                helper::setupVFRB();

                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,89.900000,0.000000,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("0"), helper::eqs);
                assert(match.str(3), std::string("22239"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("22239"), helper::eqs);
                assert(match.str(2), std::string("0"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            })->test("Cross Southpole",
            []()
            {
                config::Configuration::base_latitude = -89.900000;
                config::Configuration::base_longitude = 180.000000;
                helper::setupVFRB();

                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,-89.900000,0.000000,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("-180"), helper::eqs);
                assert(match.str(3), std::string("22239"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("-22239"), helper::eqs);
                assert(match.str(2), std::string("0"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            })->test("Cross 0-Meridian on Equator E to W",
            []()
            {
                config::Configuration::base_latitude = 0.000000;
                config::Configuration::base_longitude = 0.100000;
                helper::setupVFRB();

                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,0.000000,-0.100000,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("-90"), helper::eqs);
                assert(match.str(3), std::string("22239"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("0"), helper::eqs);
                assert(match.str(2), std::string("-22239"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            })->test("Cross 0-Meridian on Equator W to E",
            []()
            {
                config::Configuration::base_latitude = 0.000000;
                config::Configuration::base_longitude = -0.100000;
                helper::setupVFRB();

                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,0.000000,0.100000,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("90"), helper::eqs);
                assert(match.str(3), std::string("22239"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("0"), helper::eqs);
                assert(match.str(2), std::string("22239"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            })->test("Cross 0-Meridian on LAT(60) E to W",
            []()
            {
                config::Configuration::base_latitude = 60.000000;
                config::Configuration::base_longitude = 0.100000;
                helper::setupVFRB();

                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,60.000000,-0.100000,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("-90"), helper::eqs);
                assert(match.str(3), std::string("11119"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("17"), helper::eqs);
                assert(match.str(2), std::string("-11119"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            })->test("Cross 0-Meridian on LAT(-60) W to E",
            []()
            {
                config::Configuration::base_latitude = -60.000000;
                config::Configuration::base_longitude = -0.100000;
                helper::setupVFRB();

                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,-60.000000,0.100000,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("90"), helper::eqs);
                assert(match.str(3), std::string("11119"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("-17"), helper::eqs);
                assert(match.str(2), std::string("11119"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            })->test("Cross 180-Meridian on Equator E to W",
            []()
            {
                config::Configuration::base_latitude = 0.000000;
                config::Configuration::base_longitude = 179.900000;
                helper::setupVFRB();

                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,0.000000,-179.900000,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("90"), helper::eqs);
                assert(match.str(3), std::string("22239"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("0"), helper::eqs);
                assert(match.str(2), std::string("22239"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            })->test("Cross 180-Meridian on Equator W to E",
            []()
            {
                config::Configuration::base_latitude = 0.000000;
                config::Configuration::base_longitude = -179.900000;
                helper::setupVFRB();

                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,0.000000,179.900000,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("-90"), helper::eqs);
                assert(match.str(3), std::string("22239"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("0"), helper::eqs);
                assert(match.str(2), std::string("-22239"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            })->test("North America",
            []()
            {
                config::Configuration::base_latitude = 33.653124;
                config::Configuration::base_longitude = -112.692253;
                helper::setupVFRB();

                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,33.825808,-112.219232,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("66"), helper::eqs);
                assert(match.str(3), std::string("47768"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("19302"), helper::eqs);
                assert(match.str(2), std::string("43695"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            })->test("South America",
            []()
            {
                config::Configuration::base_latitude = -34.680059;
                config::Configuration::base_longitude = -58.818111;
                helper::setupVFRB();

                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,-34.699833,-58.791788,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("132"), helper::eqs);
                assert(match.str(3), std::string("3260"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("-2199"), helper::eqs);
                assert(match.str(2), std::string("2407"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            })->test("North Africa",
            []()
            {
                config::Configuration::base_latitude = 5.392435;
                config::Configuration::base_longitude = -5.748392;
                helper::setupVFRB();

                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,5.386705,-5.750365,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("-161"), helper::eqs);
                assert(match.str(3), std::string("674"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("-638"), helper::eqs);
                assert(match.str(2), std::string("-219"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            })->test("South Africa",
            []()
            {
                config::Configuration::base_latitude = -26.069244;
                config::Configuration::base_longitude = 15.484389;
                helper::setupVFRB();

                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,-23.229517,15.049683,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("-8"), helper::eqs);
                assert(match.str(3), std::string("318804"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("315692"), helper::eqs);
                assert(match.str(2), std::string("-44437"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            })->test("Australia",
            []()
            {
                config::Configuration::base_latitude = -25.278208;
                config::Configuration::base_longitude = 133.366885;
                helper::setupVFRB();

                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,-26.152199,133.376684,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("179"), helper::eqs);
                assert(match.str(3), std::string("97188"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("-97183"), helper::eqs);
                assert(match.str(2), std::string("978"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            })->test("Central Europe",
            []()
            {
                config::Configuration::base_latitude = 49.719521;
                config::Configuration::base_longitude = 9.083279;
                helper::setupVFRB();

                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.719445,9.087646,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("92"), helper::eqs);
                assert(match.str(3), std::string("314"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("-8"), helper::eqs);
                assert(match.str(2), std::string("314"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            })->test("Asia",
            []()
            {
                config::Configuration::base_latitude = 65.900837;
                config::Configuration::base_longitude = 101.570680;
                helper::setupVFRB();

                helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,32.896360,103.855837,,,,,,0", Priority::DONTCARE);
                std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
                boost::smatch match;

                bool matched = boost::regex_search(proc, match, helper::pflauRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("176"), helper::eqs);
                assert(match.str(3), std::string("3673118"), helper::eqs);

                matched = boost::regex_search(proc, match, helper::pflaaRE);
                assert(matched, true, helper::eqb);
                assert(match.str(1), std::string("-3666184"), helper::eqs);
                assert(match.str(2), std::string("225589"), helper::eqs);
                assert(match.str(3), std::string("1000"), helper::eqs);
            });
}
