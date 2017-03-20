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

#include "../src/config/Configuration.h"
#include "../src/data/AircraftContainer.h"
#include "../src/data/ClimateData.h"
#include "../src/parser/APRSParser.h"
#include "../src/parser/SBSParser.h"
#include "../src/vfrb/VFRB.h"
#include "../src/util/Math.hpp"
#include "framework/src/comparator/Comparators.hpp"
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
    for (int i = 0; i < 4; ++i)
    {
        VFRB::msAcCont.processAircrafts();
    }
}

int main(int argc, char* argv[])
{
    auto rep = reporter::createXmlReporter();
    TestSuitesRunner runner;
    Comparator<int> eqi = EQUALS<int>();
    Comparator<double> eqd = EQUALS<double>();

    SBSParser pars_sbs;
    APRSParser pars_aprs;

    VFRB::msAcCont.initProcessor(0.0, 0.0, 0);
    VFRB::msClimateData.setPress();
    VFRB::msClimateData.setTemp();

    Configuration::filter_maxHeight = INT32_MAX;
    Configuration::filter_maxDist = INT32_MAX;

    /**
     * Testplan:
     *
     * - read valid config
     * - read malformed config
     *
     * - unpack sbs ...
     * - unpack aprs ...
     *
     * - unpack wimda/wimwv
     *
     * - prefer flarms
     *
     * - filter height/dist
     *
     * - vfrb multi input (run)
     */

    describe("Math utils", runner, "Math")->test("radian", [&eqd]()
    {
        assert(Math::radian(45.0),0.785398,eqd);
        assert(Math::radian(0.0),0.0,eqd);
        assert(Math::radian(360.0),6.28319,eqd);
    })->test("degree", [&eqd]()
    {
        assert(Math::degree(0.785398),45.0,eqd);
        assert(Math::degree(0.0),0.0,eqd);
        assert(Math::degree(6.28319),360.0,eqd);
    })->test("dToI", [&eqi]()
    {
        assert(Math::dToI(0.0),0,eqi);
        assert(Math::dToI(1.4),1,eqi);
        assert(Math::dToI(1.5),2,eqi);
        assert(Math::dToI(-1.4),-1,eqi);
        assert(Math::dToI(-1.5),-2,eqi);
    })->test("dmsToDeg", [&eqd]()
    {
        assert(Math::dmsToDeg(0.0),0.0,eqd);
        assert(Math::dmsToDeg(90.303000),90.50833,eqd);
        assert(Math::dmsToDeg(180.0),180.0,eqd);
        assert(Math::dmsToDeg(-45.123456),45.2096,eqd);
    })->test("calcIcaoHeight", [&eqd]()
    {
        assert(Math::calcIcaoHeight(0.0),44330.769231,eqd);
        assert(Math::calcIcaoHeight(1013.25),0.0,eqd);
        assert(Math::calcIcaoHeight(980.0),280.578763,eqd);
    })->test("checksum", [&eqi]()
    {
        assert(Math::checksum("",sizeof("")),0,eqi);
        assert(Math::checksum("\0", sizeof("\0")),0,eqi);
        assert(Math::checksum("$abc*", sizeof("$abc*")),96,eqi);
    });

    describe<SBSParser>("SBSParser - unpack", runner)->test(
            "valid msg",
            [&]()
            {
                assert(pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0"),MSG_UNPACK_SUC,eqi);
            })->test(
            "invalid msg",
            [&]()
            {
                assert(pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,,,,,,,,0"),MSG_UNPACK_ERR,eqi);
            })->test(
            "filter height",
            [&]()
            {
                Configuration::filter_maxHeight = 0;
                assert(pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0"),MSG_UNPACK_IGN,eqi);
                Configuration::filter_maxHeight = INT32_MAX;
            })->test(
            "broken msg",
            [&]()
            {
                assert(pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0"),MSG_UNPACK_ERR,eqi);
                assert(pars_sbs.unpack("MSG,3,0,0,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0"),MSG_UNPACK_ERR,eqi);
                assert(pars_sbs.unpack(""),MSG_UNPACK_IGN,eqi);
            });

    describe<APRSParser>("APRSParser - unpack", runner)->test(
            "valid msg",
            [&]()
            {
                assert(pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00N/00800.00E'/A=001955 !W19! id06AAAAAA"),MSG_UNPACK_SUC,eqi);
                assert(pars_aprs.unpack("ICAAAAAAA>APRS,qAR:/081733h4900.00N/00800.00EX180/003/A=000495 !W38! id0DAAAAAA -138fpm +0.0rot 6.2dB 0e +4.2kHz gps4x4"),MSG_UNPACK_SUC,eqi);
                assert(pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/100715h4427.24N\\00602.18E^276/014/A=001965 !W07! id22AAAAAA -019fpm +3.7rot 37.8dB 0e -51.2kHz gps2x4"),MSG_UNPACK_SUC,eqi);
                assert(pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=000000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz"),MSG_UNPACK_SUC,eqi);
            })->test(
            "ignores",
            [&]()
            {
                assert(pars_aprs.unpack("Valhalla>APRS,TCPIP*,qAC,GLIDERN2:/074555h4900.00NI00800.00E&/A=000000 CPU:4.0 RAM:242.7/458.8MB NTP:0.8ms/-28.6ppm +56.2C RF:+38+2.4ppm/+1.7dB"),MSG_UNPACK_IGN,eqi);
                assert(pars_aprs.unpack("# aprsc 2.0.14-g28c5a6a 29 Jun 2014 07:46:15 GMT SERVER1 00.000.00.000:14580"),MSG_UNPACK_IGN,eqi);
                assert(pars_aprs.unpack(""),MSG_UNPACK_IGN,eqi);
            })->test(
            "filter height",
            [&]()
            {
                Configuration::filter_maxHeight = 0;
                assert(pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=001000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz"),MSG_UNPACK_IGN,eqi);
                Configuration::filter_maxHeight = INT32_MAX;
            });

    helper_clearAcCont();

    return rep->report(runner);
}

