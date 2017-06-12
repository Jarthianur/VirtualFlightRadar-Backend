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

#include "../../src/config/Configuration.h"
#include "../../src/data/SensorData.h"
#include "../../src/parser/AprsParser.h"
#include "../../src/parser/Parser.h"
#include "../../src/parser/SbsParser.h"
#include "../../src/parser/SensorParser.h"
#include "../../src/vfrb/VFRB.h"
#include "../framework/src/comparator/ComparatorStrategy.hpp"
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

void test_parser(TestSuitesRunner& runner)
{
    describe<parser::SbsParser>("parser::SBSParser - unpack", runner)->test(
            "valid msg",
            []()
            {
                assert(helper::pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0", 0), MSG_UNPACK_SUC, helper::eqi);
            })->test("invalid msg",
            []()
            {
                assert(helper::pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,,,,,,,,,,,0", 0), MSG_UNPACK_ERR, helper::eqi);
                assert(helper::pars_sbs.unpack("MSG,3,0,0,,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,,,,,,,,0", 0), MSG_UNPACK_IGN, helper::eqi);
                assert(helper::pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,,2017/02/16,20:11:30.772,,1000,,,,,,,,,,0", 0), MSG_UNPACK_IGN, helper::eqi);
                assert(helper::pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,,,,,,,0", 0), MSG_UNPACK_ERR, helper::eqi);
                assert(helper::pars_sbs.unpack("MSG,someCrap in, here", 0), MSG_UNPACK_IGN, helper::eqi);
                assert(helper::pars_sbs.unpack("MSG,4,0,,,,,,", 0), MSG_UNPACK_IGN, helper::eqi);
            })->test("filter height",
            []()
            {
                config::Configuration::filter_maxHeight = 0;
                assert(helper::pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0", 0), MSG_UNPACK_IGN, helper::eqi);
                config::Configuration::filter_maxHeight = INT32_MAX;
            })->test("broken msg",
            []()
            {
                assert(helper::pars_sbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0", 0), MSG_UNPACK_ERR, helper::eqi);
                assert(helper::pars_sbs.unpack("MSG,3,0,0,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0", 0), MSG_UNPACK_ERR, helper::eqi);
                assert(helper::pars_sbs.unpack("", 0), MSG_UNPACK_IGN, helper::eqi);
            });

    describe<parser::AprsParser>("parser::APRSParser - unpack", runner)->test(
            "valid msg",
            []()
            {
                assert(helper::pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00N/00800.00E'/A=000000 !W19! id06AAAAAA", 0), MSG_UNPACK_SUC, helper::eqi);
                assert(helper::pars_aprs.unpack("ICAAAAAAA>APRS,qAR:/081733h4900.00N/00800.00EX180/003/A=000000 !W38! id0DAAAAAA -138fpm +0.0rot 6.2dB 0e +4.2kHz gps4x4", 0), MSG_UNPACK_SUC, helper::eqi);
                assert(helper::pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00S\\00800.00E^276/014/A=000000 !W07! id22AAAAAA -019fpm +3.7rot 37.8dB 0e -51.2kHz gps2x4", 0), MSG_UNPACK_SUC, helper::eqi);
                assert(helper::pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=000000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz", 0), MSG_UNPACK_SUC, helper::eqi);
            })->test("ignores",
            []()
            {
                assert(helper::pars_aprs.unpack("Valhalla>APRS,TCPIP*,qAC,GLIDERN2:/074555h4900.00NI00800.00E&/A=000000 CPU:4.0 RAM:242.7/458.8MB NTP:0.8ms/-28.6ppm +56.2C RF:+38+2.4ppm/+1.7dB", 0), MSG_UNPACK_IGN, helper::eqi);
                assert(helper::pars_aprs.unpack("# aprsc 2.0.14-g28c5a6a 29 Jun 2014 07:46:15 GMT SERVER1 00.000.00.000:14580", 0), MSG_UNPACK_IGN, helper::eqi);
                assert(helper::pars_aprs.unpack("", 0), MSG_UNPACK_IGN, helper::eqi);
                assert(helper::pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00N/00800.00E'/A=000000 ", 0), MSG_UNPACK_IGN, helper::eqi);
            })->test("filter height",
            []()
            {
                config::Configuration::filter_maxHeight = 0;
                assert(helper::pars_aprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=001000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz", 0), MSG_UNPACK_IGN, helper::eqi);
                config::Configuration::filter_maxHeight = INT32_MAX;
            });

    describe<parser::SensorParser>("parser::SensorParser - unpack", runner)->test(
            "valid msg",
            []()
            {
                assert(helper::pars_wind.unpack("$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E\r\n", 0), MSG_UNPACK_SUC, helper::eqi);
                assert(helper::pars_wind.unpack("$WIMWV,242.8,R,6.9,N,A*20", 0), MSG_UNPACK_SUC, helper::eqi);
            })->test("invalid msg",
            []()
            {
                assert(helper::pars_wind.unpack("$YXXDR,C,19.3,C,BRDT,U,11.99,V,BRDV*75", 0), MSG_UNPACK_IGN, helper::eqi);
                assert(helper::pars_wind.unpack("Someone sent other stuff", 0), MSG_UNPACK_ERR, helper::eqi);
            })->test("broken msg",
            []()
            {
                assert(helper::pars_wind.unpack("$WIMDA,29.7987,I,1.0091,14.8,,,,,,,,,,,,,,*3F", 0), MSG_UNPACK_ERR, helper::eqi);
                assert(helper::pars_wind.unpack("$WIMDA,", 0), MSG_UNPACK_ERR, helper::eqi);
                assert(helper::pars_wind.unpack("$WIMDA,29.7987,I,1.0#091,B,14.8,C,,,,,,,,,,,,,,*1d", 0), MSG_UNPACK_ERR, helper::eqi);
						assert(helper::pars_wind.unpack("", 0), MSG_UNPACK_ERR, helper::eqi);
            })->test<data::SensorData>("extract WIMWV",
            []()
            {
                assert(vfrb::VFRB::msSensorData.getMwvStr(), std::string("$WIMWV,242.8,R,6.9,N,A*20\r\n"), helper::eqs);
                assert(vfrb::VFRB::msSensorData.getMwvStr(), std::string(""), helper::eqs);
            });
}
