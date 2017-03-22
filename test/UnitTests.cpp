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
#include "../src/data/AircraftContainer.h"
#include "../src/data/ClimateData.h"
#include "../src/parser/APRSParser.h"
#include "../src/parser/Parser.h"
#include "../src/parser/SBSParser.h"
#include "../src/parser/WindParser.h"
#include "../src/util/Math.hpp"
#include "../src/vfrb/VFRB.h"
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

/**
 * after that, inserted aircraft is not reported until update
 */
void helper_clearAcCont()
{
    for (int i = 0; i < AC_INVALIDATE; ++i)
    {
        VFRB::msAcCont.processAircrafts();
    }
}

int main(int argc, char *argv[])
{
    auto rep = reporter::createColoredReporter();
    TestSuitesRunner runner;
    Comparator<int> eqi = EQUALS<int>();
    Comparator<double> eqd = EQUALS<double>();
    Comparator<std::string> eqs = EQUALS<std::string>();

    boost::regex pflauRE("\\$PFLAU,,,,1,0,([-]?\\d+?)," // rel bear #1
            "0,(\\d+?),"// rel vert #2
            "(\\d+?),"// dist #3
            "(\\S{6})\\*(?:\\S{2})",// id #4
            boost::regex_constants::optimize);
    boost::regex pflaaRE("\\$PFLAA,0,([-]?\\d+?),"  // rel N #1
            "([-]?\\d+?),"// rel E #2
            "([-]?\\d+?),"// rel vert #3
            "(\\d+?),"// id type #4
            "(\\S{6}),"// id #5
            "(\\d{3})?,,"// heading #6
            "(\\d+?)?,"// gnd-speed #7
            "([-]?\\d+?\\.\\d+?)?,"// climb rate #8
            "([0-9A-F])\\*(?:\\S{2})",// aircraft type #9
            boost::regex_constants::optimize);
    boost::smatch match;

    SBSParser pars_sbs;
    APRSParser pars_aprs;
    WindParser pars_wind;

    Configuration::filter_maxHeight = INT32_MAX;
    Configuration::filter_maxDist = INT32_MAX;

    describe("Math utils", runner, "Math")->test("radian", [&eqd]()
    {
        assert(Math::radian(45.0), 0.785398, eqd);
        assert(Math::radian(0.0), 0.0, eqd);
        assert(Math::radian(360.0), 6.28319, eqd);
    })->test("degree", [&eqd]()
    {
        assert(Math::degree(0.785398), 45.0, eqd);
        assert(Math::degree(0.0), 0.0, eqd);
        assert(Math::degree(6.28319), 360.0, eqd);
    })->test("dToI", [&eqi]()
    {
        assert(Math::dToI(0.0), 0, eqi);
        assert(Math::dToI(1.4), 1, eqi);
        assert(Math::dToI(1.5), 2, eqi);
        assert(Math::dToI(-1.4), -1, eqi);
        assert(Math::dToI(-1.5), -2, eqi);
    })->test("dmsToDeg", [&eqd]()
    {
        assert(Math::dmsToDeg(0.0), 0.0, eqd);
        assert(Math::dmsToDeg(90.303000), 90.50833, eqd);
        assert(Math::dmsToDeg(180.0), 180.0, eqd);
        assert(Math::dmsToDeg(-45.123456), 45.2096, eqd);
    })->test("calcIcaoHeight", [&eqd]()
    {
        assert(Math::calcIcaoHeight(0.0), 44330.769231, eqd);
        assert(Math::calcIcaoHeight(1013.25), 0.0, eqd);
        assert(Math::calcIcaoHeight(980.0), 280.578763, eqd);
    })->test("checksum", [&eqi]()
    {
        assert(Math::checksum("", sizeof("")), 0, eqi);
        assert(Math::checksum("\0", sizeof("\0")), 0, eqi);
        assert(Math::checksum("$abc*", sizeof("$abc*")), 96, eqi);
    });

    describe<SBSParser>("SBSParser - unpack", runner)->test(
            "valid msg",
            [&]()
            {
                assert(pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0"), MSG_UNPACK_SUC, eqi);
            })->test(
            "invalid msg",
            [&]()
            {
                assert(pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,,,,,,,,,,,0"), MSG_UNPACK_ERR, eqi);
                assert(pars_sbs.unpack("MSG,3,0,0,,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,,,,,,,,0"), MSG_UNPACK_IGN, eqi);
                assert(pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,,2017/02/16,20:11:30.772,,1000,,,,,,,,,,0"), MSG_UNPACK_IGN, eqi);
                assert(pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,,,,,,,0"), MSG_UNPACK_ERR, eqi);
                assert(pars_sbs.unpack("MSG,someCrap in, here"), MSG_UNPACK_IGN, eqi);
                assert(pars_sbs.unpack("MSG,4,0,,,,,,"), MSG_UNPACK_IGN, eqi);
            })->test(
            "filter height",
            [&]()
            {
                Configuration::filter_maxHeight = 0;
                assert(pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0"), MSG_UNPACK_IGN, eqi);
                Configuration::filter_maxHeight = INT32_MAX;
            })->test(
            "broken msg",
            [&]()
            {
                assert(pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0"), MSG_UNPACK_ERR, eqi);
                assert(pars_sbs.unpack("MSG,3,0,0,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0"), MSG_UNPACK_ERR, eqi);
                assert(pars_sbs.unpack(""), MSG_UNPACK_IGN, eqi);
            });

    describe<APRSParser>("APRSParser - unpack", runner)->test(
            "valid msg",
            [&]()
            {
                assert(pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00N/00800.00E'/A=000000 !W19! id06AAAAAA"), MSG_UNPACK_SUC, eqi);
                assert(pars_aprs.unpack("ICAAAAAAA>APRS,qAR:/081733h4900.00N/00800.00EX180/003/A=000000 !W38! id0DAAAAAA -138fpm +0.0rot 6.2dB 0e +4.2kHz gps4x4"), MSG_UNPACK_SUC, eqi);
                assert(pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00S\\00800.00E^276/014/A=000000 !W07! id22AAAAAA -019fpm +3.7rot 37.8dB 0e -51.2kHz gps2x4"), MSG_UNPACK_SUC, eqi);
                assert(pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=000000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz"), MSG_UNPACK_SUC, eqi);
            })->test(
            "ignores",
            [&]()
            {
                assert(pars_aprs.unpack("Valhalla>APRS,TCPIP*,qAC,GLIDERN2:/074555h4900.00NI00800.00E&/A=000000 CPU:4.0 RAM:242.7/458.8MB NTP:0.8ms/-28.6ppm +56.2C RF:+38+2.4ppm/+1.7dB"), MSG_UNPACK_IGN, eqi);
                assert(pars_aprs.unpack("# aprsc 2.0.14-g28c5a6a 29 Jun 2014 07:46:15 GMT SERVER1 00.000.00.000:14580"), MSG_UNPACK_IGN, eqi);
                assert(pars_aprs.unpack(""), MSG_UNPACK_IGN, eqi);
                assert(pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00N/00800.00E'/A=000000 "), MSG_UNPACK_IGN, eqi);
            })->test(
            "filter height",
            [&]()
            {
                Configuration::filter_maxHeight = 0;
                assert(pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=001000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz"), MSG_UNPACK_IGN, eqi);
                Configuration::filter_maxHeight = INT32_MAX;
            });

    describe<WindParser>("WindParser - unpack", runner)->test(
            "valid msg",
            [&]()
            {
                assert(pars_wind.unpack("$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E"), MSG_UNPACK_SUC, eqi);
                assert(pars_wind.unpack("$WIMWV,242.8,R,6.9,N,A*20"), MSG_UNPACK_SUC, eqi);
            })->test(
            "invalid msg",
            [&]()
            {
                assert(pars_wind.unpack("$YXXDR,C,19.3,C,BRDT,U,11.99,V,BRDV*75"), MSG_UNPACK_IGN, eqi);
                assert(pars_wind.unpack("Someone sent other stuff"), MSG_UNPACK_IGN, eqi);
            })->test(
            "broken msg",
            [&]()
            {
                assert(pars_wind.unpack("$WIMDA,29.7987,I,1.0091,14.8,,,,,,,,,,,,,,*3E"), MSG_UNPACK_ERR, eqi);
                assert(pars_wind.unpack("$WIMDA,29.7987,I,1.0091,B,14.8,,,,,,,,,,,,,,*3E"), MSG_UNPACK_ERR, eqi);
                assert(pars_wind.unpack("$WIMDA,"), MSG_UNPACK_ERR, eqi);
                assert(pars_wind.unpack("$WIMDA,29.7987,I,1.0#091,B,14.8,C,,,,,,,,,,,,,,*3E"), MSG_UNPACK_ERR, eqi);
                assert(pars_wind.unpack("$WIMDA,29.7987,I,1.0091,B,1#4.8,C,,,,,,,,,,,,,,*3E"), MSG_UNPACK_ERR, eqi);
                assert(pars_wind.unpack(""), MSG_UNPACK_IGN, eqi);
            });

    describe<AircraftProcessor>("Process Aircrafts in N/E", runner)->test("setup", [&]()
    {
        helper_clearAcCont();
        Configuration::base_altitude = 0;
        Configuration::base_latitude = 49.000000;
        Configuration::base_longitude = 8.000000;
        Configuration::base_pressure = 1013.25;
        VFRB::msClimateData.setPress();
        VFRB::msAcCont.initProcessor(Configuration::base_latitude,
                Configuration::base_longitude,
                Configuration::base_altitude);
    })->test(
            "Aircraft at,above base pos",
            [&]()
            {
                pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.000000,8.000000,,,,,,0");
                std::string proc = VFRB::msAcCont.processAircrafts();

                bool matched = boost::regex_search(proc, match, pflauRE);
                assert(matched, true, EQUALS<bool>());

                assert(match.str(1), std::string("0"), eqs);
                assert(match.str(2), std::string("1000"), eqs);
                assert(match.str(3), std::string("0"), eqs);
                assert(match.str(4), std::string("BBBBBB"), eqs);

                matched = boost::regex_search(proc, match, pflaaRE);
                assert(matched, true, EQUALS<bool>());

                assert(match.str(1), std::string("0"), eqs);
                assert(match.str(2), std::string("0"), eqs);
                assert(match.str(3), std::string("1000"), eqs);
                assert(match.str(5), std::string("BBBBBB"), eqs);
                assert(match.str(9), std::string("8"), eqs);
            })->test(
            "Aircraft in distance",
            [&]()
            {
                pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.100000,8.100000,,,,,,0");
                std::string proc = VFRB::msAcCont.processAircrafts();

                bool matched = boost::regex_search(proc, match, pflauRE);
                assert(matched, true, EQUALS<bool>());
            });

    describe<AircraftContainer>("Container Functions", runner)->test(
            "invalidate aircraft", [&]()
            {
                helper_clearAcCont();
                assert(VFRB::msAcCont.processAircrafts(),std::string(""),eqs);
            })->test("delete aircraft", [&]()
    {  //must be verified in coverage report...
                         for (int i=0; i<30; ++i)
                         {
                             helper_clearAcCont();
                         }
                     })->test(
            "prefer FLARM, accept again if no input",
            [&]()
            {
                Configuration::base_altitude = 0;
                Configuration::base_latitude = 49.000000;
                Configuration::base_longitude = 8.000000;
                Configuration::base_pressure = 1013.25;
                VFRB::msClimateData.setPress();
                VFRB::msAcCont.initProcessor(Configuration::base_latitude,
                        Configuration::base_longitude,
                        Configuration::base_altitude);

                pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.000000,8.000000,,,,,,0");
                VFRB::msAcCont.processAircrafts();
                pars_aprs.unpack("FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=002000 id0ABBBBBB +010fpm +0.3rot");
                VFRB::msAcCont.processAircrafts();
                pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:32.000,2017/02/16,20:11:32.000,,3281,,,49.000000,8.000000,,,,,,0");
                std::string proc = VFRB::msAcCont.processAircrafts();
                bool matched = boost::regex_search(proc, match, pflauRE);
                assert(matched, true, EQUALS<bool>());

                assert(match.str(2),std::string("610"),eqs);

                helper_clearAcCont();
                pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:33.000,2017/02/16,20:11:33.000,,3281,,,49.000000,8.000000,,,,,,0");
                proc = VFRB::msAcCont.processAircrafts();
                matched = boost::regex_search(proc, match, pflauRE);
                assert(matched, true, EQUALS<bool>());

                assert(match.str(2),std::string("1000"),eqs);
            });

    return rep->report(runner);
}
