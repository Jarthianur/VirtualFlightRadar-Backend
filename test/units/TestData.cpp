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
    describe<AircraftContainer>("Container functions", runner)->test(
            "invalidate aircraft",
            []()
            {
                AircraftContainer container;
                Aircraft ac;
                GpsPosition pos =
                {   49.0, 8.0, 0};
                double press = 1013.25;

                helper::parsSbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.000000,8.000000,,,,,,0", ac);
                container.upsert(ac, 0);
                for (int i = 0; i < AC_INVALIDATE; ++i)
                {
                    container.processAircrafts(pos, press);
                }
                assert(container.processAircrafts(pos, press), std::string(""), helper::eqs);
            })->test("delete aircraft",
            []()
            {
                AircraftContainer container;
                Aircraft ac;
                GpsPosition pos =
                {   49.0, 8.0, 0};
                double press = 1013.25;

                helper::parsSbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.000000,8.000000,,,,,,0", ac);
                container.upsert(ac, 0);
                for (int i = 0; i < AC_DELETE_THRESHOLD; ++i)
                {
                    container.processAircrafts(pos, press);
                }
            })->test("prefer FLARM, accept again if no input",
            []()
            {
                config::Configuration::filter_maxDist = INT32_MAX;
                config::Configuration::filter_maxHeight = INT32_MAX;
                GpsPosition pos =
                {   49.0, 8.0, 0};
                double press = 1013.25;
                AircraftContainer container;
                boost::smatch match;
                Aircraft ac;

                helper::parsSbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.000000,8.000000,,,,,,0", ac);
                container.upsert(ac, 0);
                container.processAircrafts(pos, press);
                helper::parsAprs.unpack("FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=002000 id0ABBBBBB +010fpm +0.3rot", ac);
                container.upsert(ac, 0);
                container.processAircrafts(pos, press);
                helper::parsSbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:32.000,2017/02/16,20:11:32.000,,3281,,,49.000000,8.000000,,,,,,0", ac);
                container.upsert(ac, 0);
                std::string proc = container.processAircrafts(pos, press);
                bool matched = boost::regex_search(proc, match, helper::pflauRe);
                assert(matched, true, helper::eqb);
                assert(match.str(2), std::string("610"), helper::eqs);
                for (int i =0; i < AC_NO_FLARM_THRESHOLD; ++i)
                {
                    container.processAircrafts(pos, press);
                }
                helper::parsSbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:33.000,2017/02/16,20:11:33.000,,3281,,,49.000000,8.000000,,,,,,0", ac);
                container.upsert(ac, 0);
                proc = container.processAircrafts(pos, press);
                matched = boost::regex_search(proc, match, helper::pflauRe);
                assert(matched, true, helper::eqb);
                assert(match.str(2), std::string("1000"), helper::eqs);
            })->test("write after attempt",
            []()
            {
                config::Configuration::filter_maxDist = INT32_MAX;
                config::Configuration::filter_maxHeight = INT32_MAX;
                GpsPosition pos =
                {   49.0, 8.0, 0};
                double press = 1013.25;
                boost::smatch match;
                Aircraft ac;
                AircraftContainer container;

                helper::parsAprs.unpack("FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=002000 id0ABBBBBB +010fpm +0.3rot", ac);
                container.upsert(ac, 2);
                container.processAircrafts(pos, press);
                helper::parsAprs.unpack("FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=001000 id0ABBBBBB +010fpm +0.3rot", ac);
                container.upsert(ac, 1);
                std::string proc = container.processAircrafts(pos, press);
                bool matched = boost::regex_search(proc, match, helper::pflauRe);
                assert(matched, true, helper::eqb);
                assert(match.str(2), std::string("610"), helper::eqs);
                helper::parsAprs.unpack("FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=001000 id0ABBBBBB +010fpm +0.3rot", ac);
                container.upsert(ac, 1);
                proc = container.processAircrafts(pos, press);
                matched = boost::regex_search(proc, match, helper::pflauRe);
                assert(matched, true, helper::eqb);
                assert(match.str(2), std::string("305"), helper::eqs);
            });

    describeParallel<data::GpsData>("gps string", runner)->test("correct gps position",
            []()
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
        std::uint64_t dummy = 0;
        gps.update(pos1, 0, dummy);
        assert(gps.getBaseAlt(), 1000, helper::eqi);
        gps.update(pos2, 1, dummy);
        assert(gps.getBaseAlt(), 2000, helper::eqi);
        gps.update(pos1, 0, dummy);
        assert(gps.getBaseAlt(), 2000, helper::eqi);
    })->test("write after attempts", []()
    {
        data::GpsData gps;
        struct ExtGpsPosition pos1;
        pos1.position.altitude = 1000;
        struct ExtGpsPosition pos2;
        pos2.position.altitude = 2000;
        std::uint64_t dummy = 0;
        gps.update(pos1, 2, dummy);
        assert(gps.getBaseAlt(), 1000, helper::eqi);
        gps.update(pos2, 1, dummy);
        assert(gps.getBaseAlt(), 1000, helper::eqi);
        gps.update(pos2, 1, dummy);
        assert(gps.getBaseAlt(), 2000, helper::eqi);
    });

    describeParallel<data::WindData>("wind data", runner)->test("extract WIMWV",
            []()
            {
                struct Climate info;
                data::WindData wind;helper::parsSens.unpack("$WIMWV,242.8,R,6.9,N,A*20\r", info);
                wind.init(info.mWind);
                assert(info.hasWind(), true, helper::eqb);
                assert(wind.getMwvStr(), std::string("$WIMWV,242.8,R,6.9,N,A*20\r\n"), helper::eqs);
                assert(wind.getMwvStr(), std::string(""), helper::eqs);
            })->test("write higher priority", []()
    {
        struct Climate info =
        {
            {   ""},
            {   "", 0.0}};
        std::uint64_t dummy = 0;
        data::WindData wind;
        wind.init(info.mWind);
        assert(wind.getMwvStr(), std::string(""), helper::eqs);
        info.mWind.mwvStr = "updated";
        wind.update(info.mWind, 2, dummy);
        assert(wind.getMwvStr(), std::string("updated"), helper::eqs);
        info.mWind.mwvStr = "lower";
        wind.update(info.mWind, 1, dummy);
        assert(wind.getMwvStr(), std::string("updated"), helper::eqs);
    })->test("write after attempt", []()
    {
        struct Climate info =
        {
            {   ""},
            {   "", 0.0}};
        std::uint64_t dummy = 0;
        data::WindData wind;
        wind.init(info.mWind);
        info.mWind.mwvStr = "updated";
        wind.update(info.mWind, 2, dummy);
        assert(wind.getMwvStr(), std::string("updated"), helper::eqs);
        info.mWind.mwvStr = "lower";
        wind.update(info.mWind, 1, dummy);
        assert(wind.getMwvStr(), std::string("updated"), helper::eqs);
        wind.update(info.mWind, 1, dummy);
        assert(wind.getMwvStr(), std::string("lower"), helper::eqs);
    });

    describeParallel<data::AtmosphereData>("atmosphere data", runner)->test(
            "get WIMDA, pressure",
            []()
            {
                struct Climate info;
                helper::parsSens.unpack("$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E\r", info);
                data::AtmosphereData atm;
                atm.init(info.mAtmosphere);
                assert(info.hasAtmosphere(), true, helper::eqb);
                assert(atm.getMdaStr(), std::string("$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E\r\n"), helper::eqs);
                assert(atm.getAtmPress(), 1009.1, helper::eqd);
            })->test("write higher priority", []()
    {
        struct Climate info =
        {
            {   ""},
            {   "", 900.0}};
        std::uint64_t dummy = 0;
        data::AtmosphereData atm;
        atm.init(info.mAtmosphere);
        assert(atm.getAtmPress(), 900.0, helper::eqd);
        info.mAtmosphere.pressure = 1000.0;
        atm.update(info.mAtmosphere, 2, dummy);
        assert(atm.getAtmPress(), 1000.0, helper::eqd);
        info.mAtmosphere.pressure = 950.0;
        atm.update(info.mAtmosphere, 1, dummy);
        assert(atm.getAtmPress(), 1000.0, helper::eqd);
    })->test("write after attempt", []()
    {
        struct Climate info =
        {
            {   ""},
            {   "", 900.0}};
        std::uint64_t dummy = 0;
        data::AtmosphereData atm;
        atm.init(info.mAtmosphere);
        assert(atm.getAtmPress(), 900.0, helper::eqd);
        info.mAtmosphere.pressure = 1000.0;
        atm.update(info.mAtmosphere, 2, dummy);
        assert(atm.getAtmPress(), 1000.0, helper::eqd);
        info.mAtmosphere.pressure = 950.0;
        atm.update(info.mAtmosphere, 1, dummy);
        assert(atm.getAtmPress(), 1000.0, helper::eqd);
        atm.update(info.mAtmosphere, 1, dummy);
        assert(atm.getAtmPress(), 950.0, helper::eqd);
    });
}
