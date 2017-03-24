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

#include "../src/aircraft/AircraftProcessor.h"
#include "../src/config/Configuration.h"
#include "../src/config/ConfigReader.h"
#include "../src/data/AircraftContainer.h"
#include "../src/data/ClimateData.h"
#include "../src/parser/APRSParser.h"
#include "../src/parser/Parser.h"
#include "../src/parser/SBSParser.h"
#include "../src/parser/WindParser.h"
#include "../src/util/Math.hpp"
#include "../src/vfrb/VFRB.h"
#include "../src/util/GPSmodule.h"
#include "framework/src/comparator/Comparators.hpp"
#include "framework/src/comparator/ComparatorStrategy.hpp"
#include "framework/src/reporter/AbstractReporter.hpp"
#include "framework/src/reporter/Reporters.hpp"
#include "framework/src/testsuite/TestSuite.hpp"
#include "framework/src/testsuite/TestSuitesRunner.hpp"
#include "framework/src/util/assert.hpp"

using namespace testsuite;
using namespace comparator;

#ifdef assert
#undef assert
#endif

namespace helper
{
/**
 * after that, inserted aircraft is not reported until update
 */
void clearAcCont()
{
    for (int i = 0; i < AC_INVALIDATE; ++i)
    {
        VFRB::msAcCont.processAircrafts();
    }
}

/**
 * set vfrb statics to config
 */
void setupVFRB()
{
    VFRB::msClimateData.setPress();
    VFRB::msAcCont.initProcessor(Configuration::base_latitude,
                                 Configuration::base_longitude,
                                 Configuration::base_altitude);
}

} // helper

int main(int argc, char *argv[])
{
    auto rep = reporter::createXmlReporter();
    TestSuitesRunner runner;
    Comparator<int> eqi = EQUALS<int>();
    Comparator<double> eqd = EQUALS<double>();
    Comparator<std::string> eqs = EQUALS<std::string>();
    Comparator<bool> eqb = EQUALS<bool>();

    boost::regex pflauRE("\\$PFLAU,,,,1,0,([-]?\\d+?)," // rel bear #1
                         "0,(\\d+?),"                   // rel vert #2
                         "(\\d+?),"                     // dist #3
                         "(\\S{6})\\*(?:\\S{2})",       // id #4
                         boost::regex_constants::optimize);
    boost::regex pflaaRE("\\$PFLAA,0,([-]?\\d+?),"  // rel N #1
                         "([-]?\\d+?),"             // rel E #2
                         "([-]?\\d+?),"             // rel vert #3
                         "(\\d+?),"                 // id type #4
                         "(\\S{6}),"                // id #5
                         "(\\d{3})?,,"              // heading #6
                         "(\\d+?)?,"                // gnd-speed #7
                         "([-]?\\d+?\\.\\d+?)?,"    // climb rate #8
                         "([0-9A-F])\\*(?:\\S{2})", // aircraft type #9
                         boost::regex_constants::optimize);
    boost::smatch match;

    SBSParser pars_sbs;
    APRSParser pars_aprs;
    WindParser pars_wind;

    Configuration::filter_maxHeight = INT32_MAX;
    Configuration::filter_maxDist = INT32_MAX;

    describe("Math utils", runner, "Math")
        ->test("radian", [&eqd]() {
            assert(Math::radian(45.0), 0.785398, eqd);
            assert(Math::radian(0.0), 0.0, eqd);
            assert(Math::radian(360.0), 6.28319, eqd);
        })
        ->test("degree", [&eqd]() {
            assert(Math::degree(0.785398), 45.0, eqd);
            assert(Math::degree(0.0), 0.0, eqd);
            assert(Math::degree(6.28319), 360.0, eqd);
        })
        ->test("dToI", [&eqi]() {
            assert(Math::dToI(0.0), 0, eqi);
            assert(Math::dToI(1.4), 1, eqi);
            assert(Math::dToI(1.5), 2, eqi);
            assert(Math::dToI(-1.4), -1, eqi);
            assert(Math::dToI(-1.5), -2, eqi);
        })
        ->test("dmsToDeg", [&eqd]() {
            assert(Math::dmsToDeg(0.0), 0.0, eqd);
            assert(Math::dmsToDeg(90.303000), 90.50833, eqd);
            assert(Math::dmsToDeg(180.0), 180.0, eqd);
            assert(Math::dmsToDeg(-45.123456), 45.2096, eqd);
        })
        ->test("calcIcaoHeight", [&eqd]() {
            assert(Math::calcIcaoHeight(0.0), 44330.769231, eqd);
            assert(Math::calcIcaoHeight(1013.25), 0.0, eqd);
            assert(Math::calcIcaoHeight(980.0), 280.578763, eqd);
        })
        ->test("checksum", [&eqi]() {
            assert(Math::checksum("", sizeof("")), 0, eqi);
            assert(Math::checksum("\0", sizeof("\0")), 0, eqi);
            assert(Math::checksum("$abc*", sizeof("$abc*")), 96, eqi);
        });

    describe<SBSParser>("SBSParser - unpack", runner)
        ->test("valid msg", [&]() {
            assert(pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0"), MSG_UNPACK_SUC, eqi);
        })
        ->test("invalid msg", [&]() {
            assert(pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,,,,,,,,,,,0"), MSG_UNPACK_ERR, eqi);
            assert(pars_sbs.unpack("MSG,3,0,0,,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,,,,,,,,0"), MSG_UNPACK_IGN, eqi);
            assert(pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,,2017/02/16,20:11:30.772,,1000,,,,,,,,,,0"), MSG_UNPACK_IGN, eqi);
            assert(pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,,,,,,,0"), MSG_UNPACK_ERR, eqi);
            assert(pars_sbs.unpack("MSG,someCrap in, here"), MSG_UNPACK_IGN, eqi);
            assert(pars_sbs.unpack("MSG,4,0,,,,,,"), MSG_UNPACK_IGN, eqi);
        })
        ->test("filter height", [&]() {
            Configuration::filter_maxHeight = 0;
            assert(pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0"), MSG_UNPACK_IGN, eqi);
            Configuration::filter_maxHeight = INT32_MAX;
        })
        ->test("broken msg", [&]() {
            assert(pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0"), MSG_UNPACK_ERR, eqi);
            assert(pars_sbs.unpack("MSG,3,0,0,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0"), MSG_UNPACK_ERR, eqi);
            assert(pars_sbs.unpack(""), MSG_UNPACK_IGN, eqi);
        });

    describe<APRSParser>("APRSParser - unpack", runner)
        ->test("valid msg", [&]() {
            assert(pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00N/00800.00E'/A=000000 !W19! id06AAAAAA"), MSG_UNPACK_SUC, eqi);
            assert(pars_aprs.unpack("ICAAAAAAA>APRS,qAR:/081733h4900.00N/00800.00EX180/003/A=000000 !W38! id0DAAAAAA -138fpm +0.0rot 6.2dB 0e +4.2kHz gps4x4"), MSG_UNPACK_SUC, eqi);
            assert(pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00S\\00800.00E^276/014/A=000000 !W07! id22AAAAAA -019fpm +3.7rot 37.8dB 0e -51.2kHz gps2x4"), MSG_UNPACK_SUC, eqi);
            assert(pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=000000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz"), MSG_UNPACK_SUC, eqi);
        })
        ->test("ignores", [&]() {
            assert(pars_aprs.unpack("Valhalla>APRS,TCPIP*,qAC,GLIDERN2:/074555h4900.00NI00800.00E&/A=000000 CPU:4.0 RAM:242.7/458.8MB NTP:0.8ms/-28.6ppm +56.2C RF:+38+2.4ppm/+1.7dB"), MSG_UNPACK_IGN, eqi);
            assert(pars_aprs.unpack("# aprsc 2.0.14-g28c5a6a 29 Jun 2014 07:46:15 GMT SERVER1 00.000.00.000:14580"), MSG_UNPACK_IGN, eqi);
            assert(pars_aprs.unpack(""), MSG_UNPACK_IGN, eqi);
            assert(pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00N/00800.00E'/A=000000 "), MSG_UNPACK_IGN, eqi);
        })
        ->test("filter height", [&]() {
            Configuration::filter_maxHeight = 0;
            assert(pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=001000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz"), MSG_UNPACK_IGN, eqi);
            Configuration::filter_maxHeight = INT32_MAX;
        });

    describe<WindParser>("WindParser - unpack", runner)
        ->test("valid msg", [&]() {
            assert(pars_wind.unpack("$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E"), MSG_UNPACK_SUC, eqi);
            assert(pars_wind.unpack("$WIMWV,242.8,R,6.9,N,A*20"), MSG_UNPACK_SUC, eqi);
        })
        ->test("invalid msg", [&]() {
            assert(pars_wind.unpack("$YXXDR,C,19.3,C,BRDT,U,11.99,V,BRDV*75"), MSG_UNPACK_IGN, eqi);
            assert(pars_wind.unpack("Someone sent other stuff"), MSG_UNPACK_IGN, eqi);
        })
        ->test("broken msg", [&]() {
            assert(pars_wind.unpack("$WIMDA,29.7987,I,1.0091,14.8,,,,,,,,,,,,,,*3E"), MSG_UNPACK_ERR, eqi);
            assert(pars_wind.unpack("$WIMDA,29.7987,I,1.0091,B,14.8,,,,,,,,,,,,,,*3E"), MSG_UNPACK_ERR, eqi);
            assert(pars_wind.unpack("$WIMDA,"), MSG_UNPACK_ERR, eqi);
            assert(pars_wind.unpack("$WIMDA,29.7987,I,1.0#091,B,14.8,C,,,,,,,,,,,,,,*3E"), MSG_UNPACK_ERR, eqi);
            assert(pars_wind.unpack("$WIMDA,29.7987,I,1.0091,B,1#4.8,C,,,,,,,,,,,,,,*3E"), MSG_UNPACK_ERR, eqi);
            assert(pars_wind.unpack(""), MSG_UNPACK_IGN, eqi);
        })
        ->test<ClimateData>("extract WIMWV", [&eqs, &eqb]() {
            assert(VFRB::msClimateData.extractWV(), std::string("$WIMWV,242.8,R,6.9,N,A*20\r\n"), eqs);
            assert(VFRB::msClimateData.isValid(), false, eqb);
        });

    describe<AircraftProcessor>("Process Aircrafts", runner)
        ->test("setup", [&]() {
            helper::clearAcCont();
            Configuration::base_altitude = 0;
            Configuration::base_latitude = 49.000000;
            Configuration::base_longitude = 8.000000;
            Configuration::base_pressure = 1013.25;
            helper::setupVFRB();
        })
        ->test("Aircraft at,above base pos", [&]() {
            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.000000,8.000000,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);

            assert(match.str(1), std::string("0"), eqs);
            assert(match.str(2), std::string("1000"), eqs);
            assert(match.str(3), std::string("0"), eqs);
            assert(match.str(4), std::string("BBBBBB"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);

            assert(match.str(1), std::string("0"), eqs);
            assert(match.str(2), std::string("0"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
            assert(match.str(5), std::string("BBBBBB"), eqs);
            assert(match.str(9), std::string("8"), eqs);
        })
        ->test("filter distance", [&]() {
            Configuration::filter_maxDist = 10000;
            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.100000,8.100000,,,,,,0");
            assert(VFRB::msAcCont.processAircrafts(), std::string(""), eqs);
            Configuration::filter_maxDist = INT32_MAX;
        });

    describe<AircraftProcessor>("process relative positions", runner)
        ->test("setup", [&]() {
            helper::clearAcCont();
            Configuration::base_altitude = 0;
            Configuration::base_latitude = -0.100000;
            Configuration::base_longitude = 0.000000;
            Configuration::base_pressure = 1013.25;
            helper::setupVFRB();
        })
        ->test("Cross Equator S to N", [&]() {
            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,0.100000,0.000000,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("0"), eqs);
            assert(match.str(3), std::string("22239"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("22239"), eqs);
            assert(match.str(2), std::string("0"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        })
        ->test("Cross Equator N to S", [&]() {
            Configuration::base_latitude = 0.100000;
            Configuration::base_longitude = 0.000000;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,-0.100000,0.000000,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("180"), eqs);
            assert(match.str(3), std::string("22239"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("-22239"), eqs);
            assert(match.str(2), std::string("0"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        })
        ->test("Cross Northpole", [&]() {
            Configuration::base_latitude = 89.900000;
            Configuration::base_longitude = 180.000000;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,89.900000,0.000000,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("0"), eqs);
            assert(match.str(3), std::string("22239"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("22239"), eqs);
            assert(match.str(2), std::string("0"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        })
        ->test("Cross Southpole", [&]() {
            Configuration::base_latitude = -89.900000;
            Configuration::base_longitude = 180.000000;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,-89.900000,0.000000,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("-180"), eqs);
            assert(match.str(3), std::string("22239"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("-22239"), eqs);
            assert(match.str(2), std::string("0"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        })
        ->test("Cross 0-Meridian on Equator E to W", [&]() {
            Configuration::base_latitude = 0.000000;
            Configuration::base_longitude = 0.100000;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,0.000000,-0.100000,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("-90"), eqs);
            assert(match.str(3), std::string("22239"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("0"), eqs);
            assert(match.str(2), std::string("-22239"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        })
        ->test("Cross 0-Meridian on Equator W to E", [&]() {
            Configuration::base_latitude = 0.000000;
            Configuration::base_longitude = -0.100000;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,0.000000,0.100000,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("90"), eqs);
            assert(match.str(3), std::string("22239"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("0"), eqs);
            assert(match.str(2), std::string("22239"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        })
        ->test("Cross 0-Meridian on LAT(60) E to W", [&]() {
            Configuration::base_latitude = 60.000000;
            Configuration::base_longitude = 0.100000;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,60.000000,-0.100000,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("-90"), eqs);
            assert(match.str(3), std::string("11119"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("17"), eqs);
            assert(match.str(2), std::string("-11119"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        })
        ->test("Cross 0-Meridian on LAT(-60) W to E", [&]() {
            Configuration::base_latitude = -60.000000;
            Configuration::base_longitude = -0.100000;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,-60.000000,0.100000,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("90"), eqs);
            assert(match.str(3), std::string("11119"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("-17"), eqs);
            assert(match.str(2), std::string("11119"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        })
        ->test("Cross 180-Meridian on Equator E to W", [&]() {
            Configuration::base_latitude = 0.000000;
            Configuration::base_longitude = 179.900000;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,0.000000,-179.900000,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("90"), eqs);
            assert(match.str(3), std::string("22239"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("0"), eqs);
            assert(match.str(2), std::string("22239"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        })
        ->test("Cross 180-Meridian on Equator W to E", [&]() {
            Configuration::base_latitude = 0.000000;
            Configuration::base_longitude = -179.900000;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,0.000000,179.900000,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("-90"), eqs);
            assert(match.str(3), std::string("22239"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("0"), eqs);
            assert(match.str(2), std::string("-22239"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        })
        ->test("North America", [&]() {
            Configuration::base_latitude = 33.653124;
            Configuration::base_longitude = -112.692253;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,33.825808,-112.219232,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("66"), eqs);
            assert(match.str(3), std::string("47768"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("19302"), eqs);
            assert(match.str(2), std::string("43695"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        })
        ->test("South America", [&]() {
            Configuration::base_latitude = -34.680059;
            Configuration::base_longitude = -58.818111;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,-34.699833,-58.791788,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("132"), eqs);
            assert(match.str(3), std::string("3260"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("-2199"), eqs);
            assert(match.str(2), std::string("2407"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        })
        ->test("North Africa", [&]() {
            Configuration::base_latitude = 5.392435;
            Configuration::base_longitude = -5.748392;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,5.386705,-5.750365,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("-161"), eqs);
            assert(match.str(3), std::string("674"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("-638"), eqs);
            assert(match.str(2), std::string("-219"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        })
        ->test("South Africa", [&]() {
            Configuration::base_latitude = -26.069244;
            Configuration::base_longitude = 15.484389;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,-23.229517,15.049683,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("-8"), eqs);
            assert(match.str(3), std::string("318804"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("315692"), eqs);
            assert(match.str(2), std::string("-44437"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        })
        ->test("Australia", [&]() {
            Configuration::base_latitude = -25.278208;
            Configuration::base_longitude = 133.366885;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,-26.152199,133.376684,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("179"), eqs);
            assert(match.str(3), std::string("97188"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("-97183"), eqs);
            assert(match.str(2), std::string("978"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        })
        ->test("Central Europe", [&]() {
            Configuration::base_latitude = 49.719521;
            Configuration::base_longitude = 9.083279;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.719445,9.087646,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("92"), eqs);
            assert(match.str(3), std::string("314"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("-8"), eqs);
            assert(match.str(2), std::string("314"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        })
        ->test("Asia", [&]() {
            Configuration::base_latitude = 65.900837;
            Configuration::base_longitude = 101.570680;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,32.896360,103.855837,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();

            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("176"), eqs);
            assert(match.str(3), std::string("3673118"), eqs);

            matched = boost::regex_search(proc, match, pflaaRE);
            assert(matched, true, eqb);
            assert(match.str(1), std::string("-3666184"), eqs);
            assert(match.str(2), std::string("225589"), eqs);
            assert(match.str(3), std::string("1000"), eqs);
        });

    describe<AircraftContainer>("Container Functions", runner)
        ->test("invalidate aircraft", [&]() {
            helper::clearAcCont();
            assert(VFRB::msAcCont.processAircrafts(), std::string(""), eqs);
        })
        ->test("delete aircraft", [&]() {
            //must be verified in coverage report...
            for (int i = 0; i < 30; ++i)
            {
                helper::clearAcCont();
            }
        })
        ->test("prefer FLARM, accept again if no input", [&]() {
            Configuration::base_altitude = 0;
            Configuration::base_latitude = 49.000000;
            Configuration::base_longitude = 8.000000;
            Configuration::base_pressure = 1013.25;
            helper::setupVFRB();

            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.000000,8.000000,,,,,,0");
            VFRB::msAcCont.processAircrafts();
            pars_aprs.unpack("FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=002000 id0ABBBBBB +010fpm +0.3rot");
            VFRB::msAcCont.processAircrafts();
            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:32.000,2017/02/16,20:11:32.000,,3281,,,49.000000,8.000000,,,,,,0");
            std::string proc = VFRB::msAcCont.processAircrafts();
            bool matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);

            assert(match.str(2), std::string("610"), eqs);

            helper::clearAcCont();
            pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:33.000,2017/02/16,20:11:33.000,,3281,,,49.000000,8.000000,,,,,,0");
            proc = VFRB::msAcCont.processAircrafts();
            matched = boost::regex_search(proc, match, pflauRE);
            assert(matched, true, eqb);

            assert(match.str(2), std::string("1000"), eqs);
        });

    describe<GPSmodule>("GPSModule - gpsfix", runner)
        ->test("gps nmea", [&eqb]() {
            GPSmodule gpsm(0.0, 0.0, 0, 0.0);
            boost::regex re("\\$GPRMC,\\d{6},A,0000\\.00,N,00000\\.00,E,0,0,\\d{6},001\\.0,W\\*[0-9a-f]{2}\\s+?"
                            "\\$GPGGA,\\d{6},000\\.0000,N,00000\\.0000,E,1,05,1,0,M,0\\.0,M,,\\*[0-9a-f]{2}");
            boost::smatch match;
            bool matched = boost::regex_search(gpsm.gpsfix(), match, re);
            assert(matched, true, eqb);
        });

    describe<ConfigReader>("read config", runner)
        ->test("read", [&eqs]() {
            ConfigReader cr("test/test.ini"); // depends on $PWD
            cr.read();
            assert(cr.getProperty("latitude", "invalid"), std::string("0.000000"), eqs);
            assert(cr.getProperty("longitude", "invalid"), std::string("invalid"), eqs);
            assert(cr.getProperty("altitude", "invalid"), std::string("1000"), eqs);
            assert(cr.getProperty("geoid", "invalid"), std::string("invalid"), eqs);
            assert(cr.getProperty("nothing", "invalid"), std::string("invalid"), eqs);
        });

    return rep->report(runner);
}
