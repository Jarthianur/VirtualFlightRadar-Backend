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

#include <string>

#include <boost/regex.hpp>

#include "data/AircraftData.h"
#include "data/AtmosphereData.h"
#include "data/GpsData.h"
#include "data/WindData.h"
#include "feed/parser/AprsParser.h"
#include "feed/parser/SbsParser.h"
#include "object/impl/DateTimeImplBoost.h"

#include "helper.hpp"

using namespace data;
using namespace sctf;
using namespace object;

void test_data(test::TestSuitesRunner& runner)
{
    describe<AircraftData>("Container functions", runner)
        ->test(
            "invalidate aircraft",
            [] {
                feed::parser::SbsParser sbsParser;
                AircraftData            data;
                Aircraft                ac;
                Location                pos{49.0, 8.0, 0};
                double                  press = 1013.25;
                sbsParser.unpack(
                    "MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.000000,8.000000,,,,,,0",
                    ac);
                data.update(std::move(ac));
                for (int i = 0; i < OBJ_OUTDATED; ++i)
                {
                    data.processAircrafts(pos, press);
                }
                std::string serial;
                data.get_serialized(serial);
                assertTrue(serial.empty());
            })
        ->test(
            "delete aircraft",
            [] {
                feed::parser::SbsParser sbsParser;
                AircraftData            data;
                Aircraft                ac;
                Location                pos{49.0, 8.0, 0};
                double                  press = 1013.25;
                sbsParser.unpack(
                    "MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.000000,8.000000,,,,,,0",
                    ac);
                data.update(std::move(ac));
                for (int i = 0; i < AC_DELETE_THRESHOLD; ++i)
                {
                    data.processAircrafts(pos, press);
                }
            })
        ->test(
            "prefer FLARM, accept again if no input",
            [] {
                feed::parser::SbsParser  sbsParser;
                feed::parser::AprsParser aprsParser;
                AircraftData             data;
                boost::smatch            match;
                Aircraft                 ac;
                Location                 pos{49.0, 8.0, 0};
                double                   press = 1013.25;
                std::string              serial;

                sbsParser.unpack(
                    "MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.000000,8.000000,,,,,,0",
                    ac);
                data.update(std::move(ac));
                data.processAircrafts(pos, press);
                aprsParser.unpack(
                    "FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=002000 id0ABBBBBB +010fpm +0.3rot",
                    ac);
                data.update(std::move(ac));
                data.processAircrafts(pos, press);
                sbsParser.unpack(
                    "MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:32.000,2017/02/16,20:11:32.000,,3281,,,49.000000,8.000000,,,,,,0",
                    ac);
                data.update(std::move(ac));
                data.processAircrafts(pos, press);
                data.get_serialized(serial);
                bool matched = boost::regex_search(serial, match, helper::pflauRe);
                assertTrue(matched);
                assertEqStr(match.str(2), "610");
                for (int i = 0; i < AC_NO_FLARM_THRESHOLD; ++i)
                {
                    data.processAircrafts(pos, press);
                }
                sbsParser.unpack(
                    "MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:33.000,2017/02/16,20:11:33.000,,3281,,,49.000000,8.000000,,,,,,0",
                    ac);
                data.update(std::move(ac));
                data.processAircrafts(pos, press);
                serial.clear();
                data.get_serialized(serial);
                matched = boost::regex_search(serial, match, helper::pflauRe);
                assertTrue(matched);
                assertEqStr(match.str(2), "1000");
            })
        ->test("write after outdated", [] {
            feed::parser::AprsParser aprsParser;
            Aircraft                 ac1(1);
            Aircraft                 ac2(2);
            AircraftData             data;
            Location                 pos{49.0, 8.0, 0};
            double                   press = 1013.25;
            std::string              dest;

            aprsParser.unpack(
                "FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=002000 id0ABBBBBB +010fpm +0.3rot",
                ac2);
            aprsParser.unpack(
                "FLRBBBBBB>APRS,qAS,XXXX:/201132h4900.00N/00800.00E'180/090/A=001000 id0ABBBBBB +010fpm +0.3rot",
                ac1);
            assertTrue(data.update(std::move(ac2)));

            for (int i = 0; i < OBJ_OUTDATED; ++i)
            {
                assertFalse(data.update(std::move(ac1)));
                data.processAircrafts(pos, press);
            }
            assertTrue(data.update(std::move(ac1)));
            data.processAircrafts(pos, press);
            data.get_serialized(dest);
            boost::smatch match;
            assertTrue(boost::regex_search(dest, match, helper::pflauRe));
            assertEqStr(match.str(2), "305");
        });

    describeParallel<GpsData>("gps string", runner)
        ->test("correct gps position",
               [] {
                   GpsData     data;
                   GpsPosition pos({10.0, 85.0, 100}, 40.0);
                   std::string fix;
                   pos.set_timeStamp(Timestamp<time::DateTimeImplBoost>(
                       helper::timePlus(-1), time::Format::HHMMSS));
                   data.update(std::move(pos));
                   assertEquals(data.location().latitude, 10.0);
                   assertEquals(data.location().longitude, 85.0);
                   assertEquals(data.location().altitude, 100);
                   data.get_serialized(fix);
                   boost::smatch match;
                   bool          matched = boost::regex_search(fix, match, helper::gpsRe);
                   assertTrue(matched);
               })
        ->test("write higher priority",
               [] {
                   GpsData     data;
                   GpsPosition pos0(0);
                   GpsPosition pos1(1);
                   pos0.set_position({0.0, 0.0, 1000});
                   pos1.set_position({0.0, 0.0, 2000});
                   pos1.set_timeStamp(Timestamp<time::DateTimeImplBoost>(
                       helper::timePlus(0), time::Format::HHMMSS));
                   assertTrue(data.update(std::move(pos1)));
                   assertEquals(data.location().altitude, 2000);
                   pos0.set_timeStamp(Timestamp<time::DateTimeImplBoost>(
                       helper::timePlus(0), time::Format::HHMMSS));
                   assertFalse(data.update(std::move(pos0)));
                   assertEquals(data.location().altitude, 2000);
               })
        ->test("write after outdated", [] {
            GpsData     data;
            GpsPosition pos1(1);
            GpsPosition pos2(2);
            std::string dest;
            pos1.set_position({0.0, 0.0, 1000});
            pos2.set_position({0.0, 0.0, 2000});
            pos2.set_timeStamp(Timestamp<time::DateTimeImplBoost>(helper::timePlus(10),
                                                                       time::Format::HHMMSS));
            assertTrue(data.update(std::move(pos2)));
            assertEquals(data.location().altitude, 2000);
            pos1.set_timeStamp(Timestamp<time::DateTimeImplBoost>(helper::timePlus(20),
                                                                       time::Format::HHMMSS));
            for (int i = 0; i < OBJ_OUTDATED; ++i)
            {
                assertFalse(data.update(std::move(pos1)));
                assertEquals(data.location().altitude, 2000);
                dest.clear();
                data.get_serialized(dest);
            }
            assertTrue(data.update(std::move(pos1)));
            assertEquals(data.location().altitude, 1000);
        });

    describeParallel<WindData>("wind data", runner)
        ->test("extract WIMWV",
               [] {
                   WindData    data;
                   Wind        wind;
                   std::string dest;
                   wind.set_serialized("$WIMWV,242.8,R,6.9,N,A*20\r\n");
                   data.update(std::move(wind));
                   data.get_serialized(dest);
                   assertEqStr(dest, "$WIMWV,242.8,R,6.9,N,A*20\r\n");
                   dest.clear();
                   data.get_serialized(dest);
                   assertEqStr(dest, "");
               })
        ->test("write higher priority",
               [] {
                   WindData    data;
                   Wind        wind0(0);
                   Wind        wind1(1);
                   std::string dest;
                   wind0.set_serialized("$WIMWV,242.8,R,6.9,N,A*20\r\n");
                   wind1.set_serialized("updated");
                   assertTrue(data.update(std::move(wind0)));
                   assertTrue(data.update(std::move(wind1)));
                   data.get_serialized(dest);
                   assertEqStr(dest, "updated");
                   wind0.set_serialized("$WIMWV,242.8,R,6.9,N,A*20\r\n");
                   assertFalse(data.update(std::move(wind0)));
               })
        ->test("write after outdated", [] {
            WindData    data;
            Wind        wind1(1);
            Wind        wind2(2);
            std::string dest;
            wind1.set_serialized("lower");
            wind2.set_serialized("higher");
            assertTrue(data.update(std::move(wind2)));
            data.get_serialized(dest);
            assertEqStr(dest, "higher");
            for (int i = 0; i < OBJ_OUTDATED - 1; ++i)
            {
                assertFalse(data.update(std::move(wind1)));
                dest.clear();
                data.get_serialized(dest);
            }
            assertTrue(data.update(std::move(wind1)));
            dest.clear();
            data.get_serialized(dest);
            assertEqStr(dest, "lower");
        });

    describeParallel<AtmosphereData>("atmosphere data", runner)
        ->test("get WIMDA, pressure",
               [] {
                   AtmosphereData data;
                   Atmosphere     atm;
                   std::string    dest;
                   atm.set_pressure(1009.1);
                   atm.set_serialized("$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E\r\n");
                   data.update(std::move(atm));
                   data.get_serialized(dest);
                   assertEqStr(dest, "$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E\r\n");
                   assertEquals(data.atmPressure(), 1009.1);
               })
        ->test("write higher priority",
               [] {
                   AtmosphereData data;
                   Atmosphere     atm0(0);
                   Atmosphere     atm1(1);
                   atm0.set_pressure(1009.1);
                   atm1.set_pressure(900.0);
                   data.update(std::move(atm0));
                   assertEquals(data.atmPressure(), 1009.1);
                   data.update(std::move(atm1));
                   assertEquals(data.atmPressure(), 900.0);
               })
        ->test("write after outdated", [] {
            AtmosphereData data;
            Atmosphere     atm1(1);
            Atmosphere     atm2(2);
            std::string    dest;
            atm1.set_pressure(1009.1);
            atm2.set_pressure(900.0);
            assertTrue(data.update(std::move(atm2)));
            assertEquals(data.atmPressure(), 900.0);
            for (int i = 0; i < OBJ_OUTDATED; ++i)
            {
                assertFalse(data.update(std::move(atm1)));
                assertEquals(data.atmPressure(), 900.0);
                dest.clear();
                data.get_serialized(dest);
            }
            assertTrue(data.update(std::move(atm1)));
            assertEquals(data.atmPressure(), 1009.1);
        });
}
