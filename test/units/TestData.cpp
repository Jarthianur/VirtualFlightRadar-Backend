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

#include <boost/regex.hpp>
#include <string>

#include "../../src/aircraft/Aircraft.hpp"
#include "../../src/aircraft/AircraftContainer.h"
#include "../../src/config/Configuration.h"
#include "../../src/data/AtmosphereData.h"
#include "../../src/data/GpsData.h"
#include "../../src/util/Position.h"
#include "../../src/VFRB.h"
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
using namespace aircraft;

void test_data(TestSuitesRunner& runner)
{
    describe<AircraftContainer>("Container Functions", runner)->test(
            "invalidate aircraft", []()
            {
                helper::clearAcCont();
                assert(VFRB::msAcCont.processAircrafts(), std::string(""), helper::eqs);
            })->test("delete aircraft", []()
    {
        //just for coverage
            for (int i = 0; i < 40; ++i)
            {
                helper::clearAcCont();
            }
        })->test("prefer FLARM, accept again if no input",
            []()
            {
                config::Configuration::base_altitude = 0;
                config::Configuration::base_latitude = 49.000000;
                config::Configuration::base_longitude = 8.000000;
                config::Configuration::base_pressure = 1013.25;
                helper::setupVFRB();
                boost::smatch match;
                Aircraft ac;
                helper::parsSbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.000000,8.000000,,,,,,0", ac);
                VFRB::msAcCont.upsert(ac, 0);
                VFRB::msAcCont.processAircrafts();
                helper::parsAprs.unpack("FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=002000 id0ABBBBBB +010fpm +0.3rot", ac);
                VFRB::msAcCont.upsert(ac, 0);
                VFRB::msAcCont.processAircrafts();
                helper::parsSbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:32.000,2017/02/16,20:11:32.000,,3281,,,49.000000,8.000000,,,,,,0", ac);
                VFRB::msAcCont.upsert(ac, 0);
                std::string proc = VFRB::msAcCont.processAircrafts();
                bool matched = boost::regex_search(proc, match, helper::pflauRe);
                assert(matched, true, helper::eqb);

                assert(match.str(2), std::string("610"), helper::eqs);

                helper::clearAcCont();
                helper::parsSbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:33.000,2017/02/16,20:11:33.000,,3281,,,49.000000,8.000000,,,,,,0", ac);
                VFRB::msAcCont.upsert(ac, 0);
                proc = VFRB::msAcCont.processAircrafts();
                matched = boost::regex_search(proc, match, helper::pflauRe);
                assert(matched, true, helper::eqb);

                assert(match.str(2), std::string("1000"), helper::eqs);
            })->test("write after attempt",
            []()
            {
                helper::setupVFRB();
                boost::smatch match;
                Aircraft ac;
                helper::parsAprs.unpack("FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=002000 id0ABBBBBB +010fpm +0.3rot", ac);
                VFRB::msAcCont.upsert(ac, 1);
                VFRB::msAcCont.processAircrafts();
                helper::parsAprs.unpack("FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=001000 id0ABBBBBB +010fpm +0.3rot", ac);
                VFRB::msAcCont.upsert(ac, 0);
                std::string proc = VFRB::msAcCont.processAircrafts();
                bool matched = boost::regex_search(proc, match, helper::pflauRe);
                assert(matched, true, helper::eqb);
                assert(match.str(2), std::string("610"), helper::eqs);
                helper::parsAprs.unpack("FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=001000 id0ABBBBBB +010fpm +0.3rot", ac);
                VFRB::msAcCont.upsert(ac, 0);
                proc = VFRB::msAcCont.processAircrafts();
                matched = boost::regex_search(proc, match, helper::pflauRe);
                assert(matched, true, helper::eqb);
                assert(match.str(2), std::string("305"), helper::eqs);
            });

    describe<data::GpsData>("gps string", runner)->test("correct gps position", []()
    {
        data::GpsData gps;
        gps.init(
                {
                    {   10.0, 85.0, 100}, 5, 1, 40.0, 0.0});
        assert(gps.getBaseLat(), 10.0, helper::eqd);
        assert(gps.getBaseLong(), 85.0, helper::eqd);
        assert(gps.getBaseAlt(), 100, helper::eqi);
        std::string fix = gps.getGpsStr();
        boost::smatch match;
        bool matched = boost::regex_search(fix, match, helper::gpsRe);
        assert(matched, true, helper::eqb);
    })->test("write higher priority", []()
    {
        data::GpsData gps;
        struct ExtGpsPosition pos1;
        pos1.position.altitude = 1000;
        struct ExtGpsPosition pos2;
        pos2.position.altitude = 2000;
        gps.update(pos1, 0);
        assert(gps.getBaseAlt(), 1000, helper::eqi);
        gps.update(pos2, 1);
        assert(gps.getBaseAlt(), 2000, helper::eqi);
        gps.update(pos1, 0);
        assert(gps.getBaseAlt(), 2000, helper::eqi);
    })->test("write after attempt", []()
    {
        data::GpsData gps;
        struct ExtGpsPosition pos1;
        pos1.position.altitude = 1000;
        struct ExtGpsPosition pos2;
        pos2.position.altitude = 2000;
        gps.update(pos1, 1);
        assert(gps.getBaseAlt(), 1000, helper::eqi);
        gps.update(pos2, 0);
        assert(gps.getBaseAlt(), 1000, helper::eqi);
        gps.update(pos2, 0);
        assert(gps.getBaseAlt(), 2000, helper::eqi);
    });

    describe<data::SensorData>("sensoric data", runner)->test("extract WIMWV",
            []()
            {
                struct SensorInfo info;
                helper::parsSens.unpack("$WIMWV,242.8,R,6.9,N,A*20\r", info);
                assert(info.mwvStr, std::string("$WIMWV,242.8,R,6.9,N,A*20\r"), helper::eqs);
                VFRB::msSensorData.update(info, 0);
                assert(VFRB::msSensorData.getMwvStr(), std::string("$WIMWV,242.8,R,6.9,N,A*20\r\n"), helper::eqs);
                assert(VFRB::msSensorData.getMwvStr(), std::string(""), helper::eqs);
            })->test("extract WIMDA",
            []()
            {
                struct SensorInfo info;
                helper::parsSens.unpack("$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E\r", info);
                VFRB::msSensorData.update(info, 0);
                assert(VFRB::msSensorData.getMdaStr(), std::string("$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E\r\n"), helper::eqs);
                assert(VFRB::msSensorData.getMdaStr(), std::string(""), helper::eqs);
            })->test("write higher priority", []()
    {
        struct SensorInfo info =
        {   "", "", 900.0};
        VFRB::msSensorData.update(info, 0);
        assert(VFRB::msSensorData.getAtmPress(), 900.0, helper::eqd);
        info.press = 950.0;
        VFRB::msSensorData.update(info, 1);
        assert(VFRB::msSensorData.getAtmPress(), 950.0, helper::eqd);
        info.press = 900.0;
        VFRB::msSensorData.update(info, 0);
        assert(VFRB::msSensorData.getAtmPress(), 950.0, helper::eqd);
    })->test("write after attempt", []()
    {
        struct SensorInfo info =
        {   "", "", 900.0};
        VFRB::msSensorData.update(info, 2);
        assert(VFRB::msSensorData.getAtmPress(), 900.0, helper::eqd);
        info.press = 950.0;
        VFRB::msSensorData.update(info, 0);
        assert(VFRB::msSensorData.getAtmPress(), 900.0, helper::eqd);
        VFRB::msSensorData.update(info, 0);
        assert(VFRB::msSensorData.getAtmPress(), 950.0, helper::eqd);
    });
}
