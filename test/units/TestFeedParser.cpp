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

#include "../../src/Math.hpp"
#include "../../src/feed/parser/AprsParser.h"
#include "../../src/feed/parser/GpsParser.h"
#include "../../src/feed/parser/SbsParser.h"
#include "../../src/feed/parser/AtmosphereParser.h"
#include "../../src/object/Aircraft.h"
#include "../../src/object/Climate.hpp"
#include "../../src/object/GpsPosition.h"
#include "../Helper.hpp"
#include "../framework/src/framework.h"

#ifdef assert
#undef assert
#endif

using namespace feed::parser;
using namespace testsuite;
using namespace comparator;

void test_feed_parser(TestSuitesRunner& runner)
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
                assert(ac.getId(), std::string("AAAAAA"), helper::equalsStr);
                assert(ac.getTargetType(), object::Aircraft::TargetType::TRANSPONDER,
                       helper::equalsAtt);
                assert(ac.getPosition().altitude,
                       math::doubleToInt(math::FEET_2_M * 1000), helper::equalsInt);
                assert(ac.getPosition().latitude, 49.0, helper::equalsD);
                assert(ac.getPosition().longitude, 8.0, helper::equalsD);
                assert(ac.getFullInfoAvailable(), false, helper::equalsBool);
            })
        ->test(
            "invalid msg",
            []() {
                SbsParser sbsParser;
                object::Aircraft ac;
                assert(
                    sbsParser.unpack(
                        "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,,,,,,,,,,,0",
                        ac),
                    false, helper::equalsBool);
                assert(
                    sbsParser.unpack(
                        "MSG,3,0,0,,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,,,,,,,,0",
                        ac),
                    false, helper::equalsBool);
                assert(
                    sbsParser.unpack(
                        "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,,,,,,,0",
                        ac),
                    false, helper::equalsBool);
                assert(sbsParser.unpack("MSG,someCrap in, here", ac), false,
                       helper::equalsBool);
                assert(sbsParser.unpack("MSG,4,0,,,,,,", ac), false, helper::equalsBool);
                assert(
                    sbsParser.unpack(
                        "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0",
                        ac),
                    false, helper::equalsBool);
                assert(
                    sbsParser.unpack(
                        "MSG,3,0,0,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0",
                        ac),
                    false, helper::equalsBool);
                assert(sbsParser.unpack("", ac), false, helper::equalsBool);
            })
        ->test("filter height", []() {
            object::Aircraft ac;
            SbsParser tmpSbs(0);
            assert(
                tmpSbs.unpack(
                    "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0",
                    ac),
                false, helper::equalsBool);
        });

    describe<AprsParser>("unpack", runner)
        ->test(
            "valid msg",
            []() {
                AprsParser aprsParser;
                object::Aircraft ac;
                assert(
                    aprsParser.unpack(
                        "FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00N/00800.00E'/A=000000 !W19! id06AAAAAA",
                        ac),
                    true, helper::equalsBool);
                assert(
                    aprsParser.unpack(
                        "ICAAAAAAA>APRS,qAR:/081733h4900.00N/00800.00EX180/003/A=000000 !W38! id0DAAAAAA -138fpm +0.0rot 6.2dB 0e +4.2kHz gps4x4",
                        ac),
                    true, helper::equalsBool);
                assert(
                    aprsParser.unpack(
                        "FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00S\\00800.00E^276/014/A=000000 !W07! id22AAAAAA -019fpm +3.7rot 37.8dB 0e -51.2kHz gps2x4",
                        ac),
                    true, helper::equalsBool);
                assert(
                    aprsParser.unpack(
                        "FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=000000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz",
                        ac),
                    true, helper::equalsBool);
                assert(ac.getId(), std::string("AAAAAA"), helper::equalsStr);
                assert(ac.getTargetType(), object::Aircraft::TargetType::FLARM,
                       helper::equalsAtt);
                assert(ac.getPosition().altitude, 0, helper::equalsInt);
                assert(ac.getPosition().latitude, 49.0, helper::equalsD);
                assert(ac.getPosition().longitude, -8.0, helper::equalsD);
                assert(ac.getFullInfoAvailable(), true, helper::equalsBool);
            })
        ->test(
            "invalid msg",
            []() {
                AprsParser aprsParser;
                object::Aircraft ac;
                assert(
                    aprsParser.unpack(
                        "Valhalla>APRS,TCPIP*,qAC,GLIDERN2:/074555h4900.00NI00800.00E&/A=000000 CPU:4.0 RAM:242.7/458.8MB NTP:0.8ms/-28.6ppm +56.2C RF:+38+2.4ppm/+1.7dB",
                        ac),
                    false, helper::equalsBool);
                assert(
                    aprsParser.unpack(
                        "# aprsc 2.0.14-g28c5a6a 29 Jun 2014 07:46:15 GMT SERVER1 00.000.00.000:14580",
                        ac),
                    false, helper::equalsBool);
                assert(aprsParser.unpack("", ac), false, helper::equalsBool);
                assert(
                    aprsParser.unpack(
                        "FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00N/00800.00E'/A=000000 ",
                        ac),
                    false, helper::equalsBool);
            })
        ->test("filter height", []() {
            AprsParser tmpAprs(0);
            object::Aircraft ac;
            assert(
                tmpAprs.unpack(
                    "FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=001000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz",
                    ac),
                false, helper::equalsBool);
        });

    describe<SensorParser>("unpack", runner)
        ->test("valid msg",
               []() {
                   SensorParser sensorParser;
                   object::Climate info;
                   std::string mda(
                       "$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E\r\n");
                   std::string mwv("$WIMWV,242.8,R,6.9,N,A*20\r\n");
                   assert(sensorParser.unpack(mda, info), true, helper::equalsBool);
                   assert(sensorParser.unpack(mwv, info), true, helper::equalsBool);
                   assert(info.atmosphere.getPressure(), 1009.1, helper::equalsD);
                   assert(info.atmosphere.getSerialized(), mda, helper::equalsStr);
                   assert(info.wind.getSerialized(), mwv, helper::equalsStr);
               })
        ->test("invalid msg", []() {
            SensorParser sensorParser;
            object::Climate info;
            assert(sensorParser.unpack("$YXXDR,C,19.3,C,BRDT,U,11.99,V,BRDV*75", info),
                   false, helper::equalsBool);
            assert(sensorParser.unpack("Someone sent other stuff", info), false,
                   helper::equalsBool);
            assert(sensorParser.unpack("$WIMDA,29.7987,I,1.0091,14.8,,,,,,,,,,,,,,*3F",
                                       info),
                   false, helper::equalsBool);
            assert(sensorParser.unpack("$WIMDA,", info), false, helper::equalsBool);
            assert(sensorParser.unpack(
                       "$WIMDA,29.7987,I,1.0#091,B,14.8,C,,,,,,,,,,,,,,*1d", info),
                   false, helper::equalsBool);
            assert(sensorParser.unpack("", info), false, helper::equalsBool);
        });

    describe<GpsParser>("unpack", runner)
        ->test(
            "valid msg",
            []() {
                GpsParser gpsParser;
                object::GpsPosition pos;
                assert(
                    gpsParser.unpack(
                        "$GPGGA,183552,5000.0466,N,00815.7555,E,1,05,1,105,M,48.0,M,,*49",
                        pos),
                    true, helper::equalsBool);
                assert(
                    gpsParser.unpack(
                        "$GPGGA,183552,5000.0466,N,00815.7555,E,1,05,1,105,M,48.0,M,,*49\r\n",
                        pos),
                    true, helper::equalsBool);
                assert(
                    gpsParser.unpack(
                        "$GPGGA,183552,5000.0466,S,00815.7555,W,1,05,1,105,M,48.0,M,,*46\n",
                        pos),
                    true, helper::equalsBool);
                assert(pos.getDilution(), 1.0, helper::equalsD);
                assert(pos.getFixQuality(), 1, helper::equalsInt);
                assert(pos.getNrOfSatellites(), 5, helper::equalsInt);
                assert(pos.getGeoid(), 48.0, helper::equalsD);
                assert(pos.getPosition().latitude, -math::dmToDeg(5000.0466),
                       helper::equalsD);
                assert(pos.getPosition().longitude, -math::dmToDeg(815.7555),
                       helper::equalsD);
                assert(pos.getPosition().altitude, 105, helper::equalsInt);
            })
        ->test("invalid msg", []() {
            GpsParser gpsParser;
            object::GpsPosition pos;
            assert(
                gpsParser.unpack(
                    "$GPGGA,183552,5000.0466,N,00815.7555,E,1,05,1,105,M,48.0,M,,*59\r\n",
                    pos),
                false, helper::equalsBool);
            assert(gpsParser.unpack(
                       "$GPGGA,183552,N,00815.7555,E,1,05,1,105,M,48.0,M,,*59\r\n", pos),
                   false, helper::equalsBool);
            assert(gpsParser.unpack("$GPGGA,\r\n", pos), false, helper::equalsBool);
            assert(gpsParser.unpack("", pos), false, helper::equalsBool);
            assert(gpsParser.unpack(
                       "$GPGGA,183552,N,00815.7555,E,1,05,1,105,M,48.0,M,,*\r\n", pos),
                   false, helper::equalsBool);
        });
}
