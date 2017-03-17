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
        VFRB::ac_cont.processAircrafts();
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

    VFRB::ac_cont.initProcessor(0.0, 0.0, 0);
    VFRB::climate_data.setPress();
    VFRB::climate_data.setTemp();

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
     *
     * - math methods
     *
     * - gps fix
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
                std::string msg1(
                        "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0");
                auto res = pars_sbs.unpack(msg1);
                assert(res,MSG_UNPACK_SUC,eqi);
            })->test(
            "invalid msg",
            [&]()
            {
                std::string msg1("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,,,,,,,,0");
                auto res = pars_sbs.unpack(msg1);
                assert(res,MSG_UNPACK_ERR,eqi);
            })->test(
            "filter height",
            [&]()
            {
                Configuration::filter_maxHeight = 0;
                std::string msg1(
                        "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0");
                auto res = pars_sbs.unpack(msg1);
                assert(res,MSG_UNPACK_IGN,eqi);
                Configuration::filter_maxHeight = INT32_MAX;
            })->test(
            "broken msg",
            [&]()
            {
                std::string msg1("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0");
                auto res = pars_sbs.unpack(msg1);
                assert(res,MSG_UNPACK_ERR,eqi);
            });

    helper_clearAcCont();

    return rep->report(runner);
}

