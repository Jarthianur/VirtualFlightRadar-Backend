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

#include <stdexcept>
#include <string>

#include "feed/parser/AprsParser.h"
#include "feed/parser/AtmosphereParser.h"
#include "feed/parser/GpsParser.h"
#include "feed/parser/SbsParser.h"
#include "feed/parser/WindParser.h"
#include "object/Aircraft.h"
#include "object/Atmosphere.h"
#include "object/GpsPosition.h"
#include "object/TimeStamp.hpp"
#include "object/Wind.h"
#include "util/math.hpp"
#include "helper.hpp"

using namespace feed::parser;
using namespace sctf;

void test_feed_parser(test::TestSuitesRunner& runner)
{
    describe<SbsParser>("unpack", runner)
        ->test(
            "valid msg",
            []() {
                SbsParser sbsParser;
                object::Aircraft ac;
                sbsParser.unpack(
                    "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0",
                    ac);
                assertEqStr(ac.get_id(), "AAAAAA");
                assertEquals(ac.get_targetType(), object::Aircraft::TargetType::TRANSPONDER);
                assertEquals(ac.get_position().altitude, math::doubleToInt(math::FEET_2_M * 1000));
                assertEquals(ac.get_position().latitude, 49.0);
                assertEquals(ac.get_position().longitude, 8.0);
                assertFalse(ac.get_fullInfo());
            })
        ->test(
            "invalid msg",
            []() {
                SbsParser sbsParser;
                object::Aircraft ac;
                assertFalse(sbsParser.unpack(
                    "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,,,,,,,,,,,0",
                    ac));
                assertFalse(sbsParser.unpack(
                    "MSG,3,0,0,,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,,,,,,,,0",
                    ac));
                assertFalse(sbsParser.unpack(
                    "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,,,,,,,0",
                    ac));
                assertFalse(sbsParser.unpack("MSG,someCrap in, here", ac));
                assertFalse(sbsParser.unpack("MSG,4,0,,,,,,", ac));
                assertFalse(sbsParser.unpack(
                    "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0",
                    ac));
                assertFalse(sbsParser.unpack(
                    "MSG,3,0,0,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0",
                    ac));
                assertFalse(sbsParser.unpack("", ac));
            })
        ->test("filter height", []() {
            object::Aircraft ac;
            SbsParser tmpSbs;
            SbsParser::s_maxHeight = 0;
            assertFalse(tmpSbs.unpack(
                "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0",
                ac));
        });

    describe<AprsParser>("unpack", runner)
        ->test(
            "valid msg",
            []() {
                AprsParser aprsParser;
                object::Aircraft ac;
                assertTrue(aprsParser.unpack(
                    "FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00N/00800.00E'/A=000000 !W19! id06AAAAAA",
                    ac));
                assertTrue(aprsParser.unpack(
                    "ICAAAAAAA>APRS,qAR:/081733h4900.00N/00800.00EX180/003/A=000000 !W38! id0DAAAAAA -138fpm +0.0rot 6.2dB 0e +4.2kHz gps4x4",
                    ac));
                assertTrue(aprsParser.unpack(
                    "FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00S\\00800.00E^276/014/A=000000 !W07! id22AAAAAA -019fpm +3.7rot 37.8dB 0e -51.2kHz gps2x4",
                    ac));
                assertTrue(aprsParser.unpack(
                    "FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=000000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz",
                    ac));
                assertEqStr(ac.get_id(), "AAAAAA");
                assertEquals(ac.get_targetType(), object::Aircraft::TargetType::FLARM);
                assertEquals(ac.get_position().altitude, 0);
                assertEquals(ac.get_position().latitude, 49.0);
                assertEquals(ac.get_position().longitude, -8.0);
                assertTrue(ac.get_fullInfo());
            })
        ->test(
            "invalid msg",
            []() {
                AprsParser aprsParser;
                object::Aircraft ac;
                assertFalse(aprsParser.unpack(
                    "Valhalla>APRS,TCPIP*,qAC,GLIDERN2:/074555h4900.00NI00800.00E&/A=000000 CPU:4.0 RAM:242.7/458.8MB NTP:0.8ms/-28.6ppm +56.2C RF:+38+2.4ppm/+1.7dB",
                    ac));
                assertFalse(aprsParser.unpack(
                    "# aprsc 2.0.14-g28c5a6a 29 Jun 2014 07:46:15 GMT SERVER1 00.000.00.000:14580",
                    ac));
                assertFalse(aprsParser.unpack("", ac));
                assertFalse(aprsParser.unpack(
                    "FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00N/00800.00E'/A=000000 ", ac));
            })
        ->test("filter height", []() {
            AprsParser tmpAprs;
            AprsParser::s_maxHeight = 0;
            object::Aircraft ac;
            assertFalse(tmpAprs.unpack(
                "FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=001000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz",
                ac));
        });

    describe<WindParser>("unpack", runner)
        ->test("valid msg",
               []() {
                   WindParser windParser;
                   object::Wind wind;
                   std::string mwv("$WIMWV,242.8,R,6.9,N,A*20\r\n");
                   assertTrue(windParser.unpack(mwv, wind));
                   assertEquals(wind.get_serialized(), mwv);
               })
        ->test("invalid msg", []() {
            WindParser windParser;
            object::Wind wind;
            assertFalse(windParser.unpack("$YXXDR,C,19.3,C,BRDT,U,11.99,V,BRDV*75", wind));
            assertFalse(windParser.unpack("Someone sent other stuff", wind));
            assertFalse(windParser.unpack("", wind));
        });

    describe<AtmosphereParser>("unpack", runner)
        ->test("valid msg",
               []() {
                   AtmosphereParser atmParser;
                   object::Atmosphere atm;
                   std::string mda("$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E\r\n");
                   assertTrue(atmParser.unpack(mda, atm));
                   assertEquals(atm.get_pressure(), 1009.1);
                   assertEquals(atm.get_serialized(), mda);
               })
        ->test("invalid msg", []() {
            AtmosphereParser atmParser;
            object::Atmosphere atm;
            assertFalse(atmParser.unpack("$YXXDR,C,19.3,C,BRDT,U,11.99,V,BRDV*75", atm));
            assertFalse(atmParser.unpack("Someone sent other stuff", atm));
            assertFalse(atmParser.unpack("$WIMDA,29.7987,I,1.0091,14.8,,,,,,,,,,,,,,*3F", atm));
            assertFalse(atmParser.unpack("$WIMDA,", atm));
            assertFalse(
                atmParser.unpack("$WIMDA,29.7987,I,1.0#091,B,14.8,C,,,,,,,,,,,,,,*1d", atm));
            assertFalse(atmParser.unpack("", atm));
        });

    describe<GpsParser>("unpack", runner)
        ->test("valid msg",
               []() {
                   GpsParser gpsParser;
                   object::GpsPosition pos;
                   assertTrue(gpsParser.unpack(
                       "$GPGGA,183552,5000.0466,N,00815.7555,E,1,05,1,105,M,48.0,M,,*49", pos));
                   assertTrue(gpsParser.unpack(
                       "$GPGGA,183552,5000.0466,N,00815.7555,E,1,05,1,105,M,48.0,M,,*49\r\n", pos));
                   assertTrue(gpsParser.unpack(
                       "$GPGGA,183552,5000.0466,S,00815.7555,W,1,05,1,105,M,48.0,M,,*46\n", pos));
                   assertEquals(pos.get_dilution(), 1.0);
                   assertEquals(pos.get_fixQuality(), 1);
                   assertEquals(pos.get_nrOfSatellites(), 5);
                   assertEquals(pos.get_geoid(), 48.0);
                   assertEquals(pos.get_position().latitude, -math::dmToDeg(5000.0466));
                   assertEquals(pos.get_position().longitude, -math::dmToDeg(815.7555));
                   assertEquals(pos.get_position().altitude, 105);
               })
        ->test("invalid msg", []() {
            GpsParser gpsParser;
            object::GpsPosition pos;
            assertFalse(gpsParser.unpack(
                "$GPGGA,183552,5000.0466,N,00815.7555,E,1,05,1,105,M,48.0,M,,*59\r\n", pos));
            assertFalse(
                gpsParser.unpack("$GPGGA,183552,N,00815.7555,E,1,05,1,105,M,48.0,M,,*59\r\n", pos));
            assertFalse(gpsParser.unpack("$GPGGA,\r\n", pos));
            assertFalse(gpsParser.unpack("", pos));
            assertFalse(
                gpsParser.unpack("$GPGGA,183552,N,00815.7555,E,1,05,1,105,M,48.0,M,,*\r\n", pos));
        });
}
