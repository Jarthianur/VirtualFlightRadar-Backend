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

#include "../../src/data/AircraftData.h"
#include "../../src/data/AtmosphereData.h"
#include "../../src/data/GpsData.h"
#include "../../src/data/WindData.h"
#include "../../src/feed/parser/AprsParser.h"
#include "../../src/feed/parser/SbsParser.h"
#include "../Helper.hpp"

using namespace data;
using namespace sctf;

void test_data(test::TestSuitesRunner& runner)
{
    describe<AircraftData>("Container functions", runner)
        ->test(
            "invalidate aircraft",
            []() {
                feed::parser::SbsParser sbsParser;
                AircraftData data;
                object::Aircraft ac;
                object::Position pos{49.0, 8.0, 0};
                double press = 1013.25;
                sbsParser.unpack(
                    "MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.000000,8.000000,,,,,,0",
                    ac);
                data.update(std::move(ac));
                for(int i = 0; i < OBJ_OUTDATED; ++i)
                {
                    data.processAircrafts(pos, press);
                }
                assertTrue(data.getSerialized().empty());
            })
        ->test(
            "delete aircraft",
            []() {
                feed::parser::SbsParser sbsParser;
                AircraftData data;
                object::Aircraft ac;
                object::Position pos{49.0, 8.0, 0};
                double press = 1013.25;
                sbsParser.unpack(
                    "MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.000000,8.000000,,,,,,0",
                    ac);
                data.update(std::move(ac));
                for(int i = 0; i < AC_DELETE_THRESHOLD; ++i)
                {
                    data.processAircrafts(pos, press);
                }
            })
        ->test(
            "prefer FLARM, accept again if no input",
            []() {
                feed::parser::SbsParser sbsParser;
                feed::parser::AprsParser aprsParser;
                AircraftData data;
                boost::smatch match;
                object::Aircraft ac;
                object::Position pos{49.0, 8.0, 0};
                double press = 1013.25;

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
                std::string proc = data.getSerialized();
                bool matched     = boost::regex_search(proc, match, helper::pflauRe);
                assertTrue(matched);
                assertStr(match.str(2), "610", EQUALS);
                for(int i = 0; i < AC_NO_FLARM_THRESHOLD; ++i)
                {
                    data.processAircrafts(pos, press);
                }
                sbsParser.unpack(
                    "MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:33.000,2017/02/16,20:11:33.000,,3281,,,49.000000,8.000000,,,,,,0",
                    ac);
                data.update(std::move(ac));
                data.processAircrafts(pos, press);
                proc    = data.getSerialized();
                matched = boost::regex_search(proc, match, helper::pflauRe);
                assertTrue(matched);
                assertStr(match.str(2), "1000", EQUALS);
            })
        ->test("write after outdated", []() {
            feed::parser::AprsParser aprsParser;
            object::Aircraft ac1(1);
            object::Aircraft ac2(2);
            AircraftData data;
            object::Position pos{49.0, 8.0, 0};
            double press = 1013.25;
            aprsParser.unpack(
                "FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=002000 id0ABBBBBB +010fpm +0.3rot",
                ac2);
            aprsParser.unpack(
                "FLRBBBBBB>APRS,qAS,XXXX:/201132h4900.00N/00800.00E'180/090/A=001000 id0ABBBBBB +010fpm +0.3rot",
                ac1);
            assertTrue(data.update(std::move(ac2)));

            for(int i = 0; i < OBJ_OUTDATED - 1; ++i)
            {
                assertFalse(data.update(std::move(ac1)));
                data.processAircrafts(pos, press);
                std::string proc = data.getSerialized();
                boost::smatch match;
                assertTrue(boost::regex_search(proc, match, helper::pflauRe));
                assertStr(match.str(2), "610", EQUALS);
            }
            data.processAircrafts(pos, press);
            assertZero(data.getSerialized().size());
            assertTrue(data.update(std::move(ac1)));
            data.processAircrafts(pos, press);
            std::string proc = data.getSerialized();
            boost::smatch match;
            assertTrue(boost::regex_search(proc, match, helper::pflauRe));
            assertStr(match.str(2), "305", EQUALS);
        });

    describeParallel<GpsData>("gps string", runner)
        ->test("correct gps position",
               []() {
                   GpsData data;
                   object::GpsPosition pos({10.0, 85.0, 100}, 40.0);
                   pos.setTimeStamp(object::TimeStamp("000001", object::TimeStamp::Format::HHMMSS));
                   data.update(std::move(pos));
                   assertEquals(data.getPosition().latitude, 10.0);
                   assertEquals(data.getPosition().longitude, 85.0);
                   assertEquals(data.getPosition().altitude, 100);
                   std::string fix = data.getSerialized();
                   boost::smatch match;
                   bool matched = boost::regex_search(fix, match, helper::gpsRe);
                   assertTrue(matched);
               })
        ->test(
            "write higher priority",
            []() {
                GpsData data;
                object::GpsPosition pos0(0);
                object::GpsPosition pos1(1);
                pos0.setPosition({0.0, 0.0, 1000});
                pos1.setPosition({0.0, 0.0, 2000});
                pos1.setTimeStamp(object::TimeStamp("000001", object::TimeStamp::Format::HHMMSS));
                assertTrue(data.update(std::move(pos0)));
                assertEquals(data.getPosition().altitude, 1000);
                data.update(std::move(pos1));
                assertEquals(data.getPosition().altitude, 2000);
                pos0.setTimeStamp(object::TimeStamp("000002", object::TimeStamp::Format::HHMMSS));
                data.update(std::move(pos0));
                assertEquals(data.getPosition().altitude, 2000);
            })
        ->test("write after outdated", []() {
            GpsData data;
            object::GpsPosition pos1(1);
            object::GpsPosition pos2(2);
            pos1.setPosition({0.0, 0.0, 1000});
            pos2.setPosition({0.0, 0.0, 2000});
            pos1.setTimeStamp(object::TimeStamp("000001", object::TimeStamp::Format::HHMMSS));
            assertTrue(data.update(std::move(pos2)));
            assertEquals(data.getPosition().altitude, 2000);

            for(int i = 0; i < OBJ_OUTDATED - 1; ++i)
            {
                assertFalse(data.update(std::move(pos1)));
                assertEquals(data.getPosition().altitude, 2000);
                data.getSerialized();
            }
            data.getSerialized();
            assertTrue(data.update(std::move(pos1)));
            assertEquals(data.getPosition().altitude, 1000);
        });

    describeParallel<WindData>("wind data", runner)
        ->test("extract WIMWV",
               []() {
                   WindData data;
                   object::Wind wind;
                   wind.set_serialized("$WIMWV,242.8,R,6.9,N,A*20\r\n");
                   data.update(std::move(wind));
                   assertStr(data.getSerialized(), "$WIMWV,242.8,R,6.9,N,A*20\r\n", EQUALS);
                   assertStr(data.getSerialized(), "", EQUALS);
               })
        ->test("write higher priority",
               []() {
                   WindData data;
                   object::Wind wind0(0);
                   object::Wind wind1(1);
                   wind0.set_serialized("$WIMWV,242.8,R,6.9,N,A*20\r\n");
                   wind1.set_serialized("updated");
                   assertTrue(data.update(std::move(wind0)));
                   assertTrue(data.update(std::move(wind1)));
                   assertStr(data.getSerialized(), "updated", EQUALS);
                   wind0.set_serialized("$WIMWV,242.8,R,6.9,N,A*20\r\n");
                   assertFalse(data.update(std::move(wind0)));
               })
        ->test("write after attempt", []() {
            WindData data;
            object::Wind wind1(1);
            object::Wind wind2(2);
            wind1.set_serialized("lower");
            wind2.set_serialized("higher");
            data.update(std::move(wind2));
            assertStr(data.getSerialized(), "higher", EQUALS);
            data.update(std::move(wind1));
            assertStr(data.getSerialized(), "", EQUALS);
            data.update(std::move(wind1));
            assertStr(data.getSerialized(), "lower", EQUALS);
        });

    describeParallel<AtmosphereData>("atmosphere data", runner)
        ->test("get WIMDA, pressure",
               []() {
                   AtmosphereData data;
                   object::Atmosphere atm;
                   atm.setPressure(1009.1);
                   atm.set_serialized("$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E\r\n");
                   data.update(std::move(atm));
                   assertStr(data.getSerialized(),
                             "$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E\r\n", EQUALS);
                   assertEquals(data.getAtmPressure(), 1009.1);
               })
        ->test("write higher priority",
               []() {
                   AtmosphereData data;
                   object::Atmosphere atm0(0);
                   object::Atmosphere atm1(1);
                   atm0.setPressure(1009.1);
                   atm1.setPressure(900.0);
                   data.update(std::move(atm0));
                   assertEquals(data.getAtmPressure(), 1009.1);
                   data.update(std::move(atm1));
                   assertEquals(data.getAtmPressure(), 900.0);
               })
        ->test("write after attempt", []() {
            AtmosphereData data;
            object::Atmosphere atm1(1);
            object::Atmosphere atm2(2);
            atm1.setPressure(1009.1);
            atm2.setPressure(900.0);
            data.update(std::move(atm2));
            assertEquals(data.getAtmPressure(), 900.0);
            data.update(std::move(atm1));
            assertEquals(data.getAtmPressure(), 900.0);
            data.update(std::move(atm1));
            assertEquals(data.getAtmPressure(), 1009.1);
        });
}
