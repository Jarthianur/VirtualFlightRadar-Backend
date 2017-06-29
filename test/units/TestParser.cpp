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

#include <boost/optional.hpp>
#include <cstdint>
#include <stdexcept>
#include <string>

#include "../../src/aircraft/Aircraft.hpp"
#include "../../src/config/Configuration.h"
#include "../../src/util/Math.hpp"
#include "../framework/src/comparator/Comparators.hpp"
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
    describe<parser::SbsParser>("unpack", runner)->test("valid msg",
            []()
            {
                aircraft::Aircraft ac;
                helper::parsSbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0", ac);
                assert(ac.getId(), std::string("AAAAAA"), helper::eqs);
                assert((std::int32_t) ac.getTargetT(), (std::int32_t) aircraft::Aircraft::TargetType::TRANSPONDER, helper::eqi);
                assert(ac.getAltitude(), math::dToI(math::FEET_2_M * 1000), helper::eqi);
                assert(ac.getLatitude(), 49.0, helper::eqd);
                assert(ac.getLongitude(), 8.0, helper::eqd);
                assert(ac.isFullInfo(), false, helper::eqb);
            })->test("invalid msg",
            []()
            {
                aircraft::Aircraft ac;
                assert(helper::parsSbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,,,,,,,,,,,0", ac), false, helper::eqb);
                assert(helper::parsSbs.unpack("MSG,3,0,0,,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,,,,,,,,0", ac), false, helper::eqb);
                assert(helper::parsSbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,,,,,,,0", ac), false, helper::eqb);
                assert(helper::parsSbs.unpack("MSG,someCrap in, here", ac), false, helper::eqb);
                assert(helper::parsSbs.unpack("MSG,4,0,,,,,,", ac), false, helper::eqb);
                assert(helper::parsSbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0", ac), false, helper::eqb);
                assert(helper::parsSbs.unpack("MSG,3,0,0,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0", ac), false, helper::eqb);
                assert(helper::parsSbs.unpack("", ac), false, helper::eqb);
            })->test("filter height",
            []()
            {
                config::Configuration::filter_maxHeight = 0;
                aircraft::Aircraft ac;
                assert(helper::parsSbs.unpack("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0", ac), false, helper::eqb);
                config::Configuration::filter_maxHeight = INT32_MAX;
            });

    describe<parser::AprsParser>("unpack", runner)->test("valid msg",
            []()
            {
                aircraft::Aircraft ac;
                assert(helper::parsAprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00N/00800.00E'/A=000000 !W19! id06AAAAAA", ac), true, helper::eqb);
                assert(helper::parsAprs.unpack("ICAAAAAAA>APRS,qAR:/081733h4900.00N/00800.00EX180/003/A=000000 !W38! id0DAAAAAA -138fpm +0.0rot 6.2dB 0e +4.2kHz gps4x4", ac), true, helper::eqb);
                assert(helper::parsAprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00S\\00800.00E^276/014/A=000000 !W07! id22AAAAAA -019fpm +3.7rot 37.8dB 0e -51.2kHz gps2x4", ac), true, helper::eqb);
                assert(helper::parsAprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=000000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz", ac), true, helper::eqb);
                assert(ac.getId(), std::string("AAAAAA"), helper::eqs);
                assert((std::int32_t) ac.getTargetT(), (std::int32_t) aircraft::Aircraft::TargetType::FLARM, helper::eqi);
                assert(ac.getAltitude(), 0, helper::eqi);
                assert(ac.getLatitude(), 49.0, helper::eqd);
                assert(ac.getLongitude(), -8.0, helper::eqd);
                assert(ac.isFullInfo(), true, helper::eqb);
            })->test("invalid msg",
            []()
            {
                aircraft::Aircraft ac;
                assert(helper::parsAprs.unpack("Valhalla>APRS,TCPIP*,qAC,GLIDERN2:/074555h4900.00NI00800.00E&/A=000000 CPU:4.0 RAM:242.7/458.8MB NTP:0.8ms/-28.6ppm +56.2C RF:+38+2.4ppm/+1.7dB", ac), false, helper::eqb);
                assert(helper::parsAprs.unpack("# aprsc 2.0.14-g28c5a6a 29 Jun 2014 07:46:15 GMT SERVER1 00.000.00.000:14580", ac), false, helper::eqb);
                assert(helper::parsAprs.unpack("", ac), false, helper::eqb);
                assert(helper::parsAprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00N/00800.00E'/A=000000 ", ac), false, helper::eqb);
            })->test("filter height",
            []()
            {
                config::Configuration::filter_maxHeight = 0;
                aircraft::Aircraft ac;
                assert(helper::parsAprs.unpack("FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=001000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz", ac), false, helper::eqb);
                config::Configuration::filter_maxHeight = INT32_MAX;
            });

    describe<parser::SensorParser>("unpack", runner)->test("valid msg", []()
    {
        struct SensorInfo info;
        std::string mda("$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E\r\n");
        std::string mwv("$WIMWV,242.8,R,6.9,N,A*20");
        assert(helper::parsSens.unpack(mda, info), true, helper::eqb);
        assert(helper::parsSens.unpack(mwv, info), true, helper::eqb);
        assert(info.press, 1009.1, helper::eqd);
        assert(info.mdaStr, mda, helper::eqs);
        assert(info.mwvStr, mwv, helper::eqs);
    })->test("invalid msg",
            []()
            {
                struct SensorInfo info;
                assert(helper::parsSens.unpack("$YXXDR,C,19.3,C,BRDT,U,11.99,V,BRDV*75", info), false, helper::eqb);
                assert(helper::parsSens.unpack("Someone sent other stuff", info), false, helper::eqb);
                assert(helper::parsSens.unpack("$WIMDA,29.7987,I,1.0091,14.8,,,,,,,,,,,,,,*3F", info), false, helper::eqb);
                assert(helper::parsSens.unpack("$WIMDA,", info), false, helper::eqb);
                assert(helper::parsSens.unpack("$WIMDA,29.7987,I,1.0#091,B,14.8,C,,,,,,,,,,,,,,*1d", info), false, helper::eqb);
                assert(helper::parsSens.unpack("", info), false, helper::eqb);
            });

    describe<parser::GpsParser>("unpack", runner)->test("valid msg",
            []()
            {
                struct ExtGpsPosition pos;
                assert(helper::parsGps.unpack("$GPGGA,183552,5000.0466,N,00815.7555,E,1,05,1,105,M,48.0,M,,*49\r\n", pos), true, helper::eqb);
                assert(helper::parsGps.unpack("$GPGGA,183552,5000.0466,S,00815.7555,W,1,05,1,105,M,48.0,M,,*46\r\n", pos), true, helper::eqb);
                assert(pos.dilution, 1.0, helper::eqd);
                assert(pos.fixQa, 1, helper::eqi);
                assert(pos.nrSats, 5, helper::eqi);
                assert(pos.geoid, 48.0, helper::eqd);
                assert(pos.position.latitude, -math::dmToDeg(5000.0466), helper::eqd);
                assert(pos.position.longitude, -math::dmToDeg(815.7555), helper::eqd);
                assert(pos.position.altitude, 105, helper::eqi);
            })->test("invalid msg",
            []()
            {
                struct ExtGpsPosition pos;
                assert(helper::parsGps.unpack("$GPGGA,183552,5000.0466,N,00815.7555,E,1,05,1,105,M,48.0,M,,*59\r\n", pos), false, helper::eqb);
                assert(helper::parsGps.unpack("$GPGGA,183552,N,00815.7555,E,1,05,1,105,M,48.0,M,,*59\r\n", pos), false, helper::eqb);
                assert(helper::parsGps.unpack("$GPGGA,\r\n", pos), false, helper::eqb);
                assert(helper::parsGps.unpack("", pos), false, helper::eqb);
                assert(helper::parsGps.unpack("$GPGGA,183552,N,00815.7555,E,1,05,1,105,M,48.0,M,,*\r\n", pos), false, helper::eqb);
            });
}
