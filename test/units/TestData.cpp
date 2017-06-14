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

#include <string>
#include <boost/regex.hpp>

#include "../../src/config/Configuration.h"
#include "../../src/data/AircraftContainer.h"
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

void test_data(TestSuitesRunner& runner) {
	describe<data::AircraftContainer>("Container Functions", runner)->test(
			"invalidate aircraft",
			[]()
			{
				helper::clearAcCont();
				assert(vfrb::VFRB::msAcCont.processAircrafts(), std::string(""), helper::eqs);
			})->test("delete aircraft", []()
	{
		//just for coverage
			for (int i = 0; i < 30; ++i)
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

				helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,3281,,,49.000000,8.000000,,,,,,0", 0);
				vfrb::VFRB::msAcCont.processAircrafts();
				helper::pars_aprs.unpack("FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=002000 id0ABBBBBB +010fpm +0.3rot", 0);
				vfrb::VFRB::msAcCont.processAircrafts();
				helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:32.000,2017/02/16,20:11:32.000,,3281,,,49.000000,8.000000,,,,,,0", 0);
				std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
				bool matched = boost::regex_search(proc, match, helper::pflauRe);
				assert(matched, true, helper::eqb);

				assert(match.str(2), std::string("610"), helper::eqs);

				helper::clearAcCont();
				helper::pars_sbs.unpack("MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:33.000,2017/02/16,20:11:33.000,,3281,,,49.000000,8.000000,,,,,,0", 0);
				proc = vfrb::VFRB::msAcCont.processAircrafts();
				matched = boost::regex_search(proc, match, helper::pflauRe);
				assert(matched, true, helper::eqb);

				assert(match.str(2), std::string("1000"), helper::eqs);
			})->test("write after attempt",
			[]() {
				helper::setupVFRB();
				boost::smatch match;
				helper::pars_aprs.unpack("FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=002000 id0ABBBBBB +010fpm +0.3rot", 1);
				vfrb::VFRB::msAcCont.processAircrafts();
				helper::pars_aprs.unpack("FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=001000 id0ABBBBBB +010fpm +0.3rot", 0);
				std::string proc = vfrb::VFRB::msAcCont.processAircrafts();
				bool matched = boost::regex_search(proc, match, helper::pflauRe);
				assert(matched, true, helper::eqb);
				assert(match.str(2), std::string("610"), helper::eqs);
				helper::pars_aprs.unpack("FLRBBBBBB>APRS,qAS,XXXX:/201131h4900.00N/00800.00E'180/090/A=001000 id0ABBBBBB +010fpm +0.3rot", 0);
				proc = vfrb::VFRB::msAcCont.processAircrafts();
				matched = boost::regex_search(proc, match, helper::pflauRe);
				assert(matched, true, helper::eqb);
				assert(match.str(2), std::string("305"), helper::eqs);
			});

	describe<data::GpsData>("gps string", runner)->test("correct gps position",
			[]() {
				data::GpsData gps;
				gps.setDefaults(10.0, 85.0,100,40.0);
				assert(gps.getBaseLat(), 10.0, helper::eqd);
				assert(gps.getBaseLong(), 85.0, helper::eqd);
				assert(gps.getBaseAlt(), 100, helper::eqi);
				std::string fix = gps.getGpsStr();
				boost::smatch match;
				bool matched = boost::regex_search(fix, match, helper::gpsRe);
				assert(matched, true, helper::eqb);
			})->test("write higher priority", []() {
		data::GpsData gps;
		struct ExtGPSPosition pos1;
		pos1.position.altitude = 1000;
		struct ExtGPSPosition pos2;
		pos2.position.altitude = 2000;
		gps.setBasePos(0, pos1);
		assert(gps.getBaseAlt(), 1000, helper::eqi);
		gps.setBasePos(1, pos2);
		assert(gps.getBaseAlt(), 2000, helper::eqi);
		gps.setBasePos(0, pos1);
		assert(gps.getBaseAlt(), 2000, helper::eqi);
	})->test("write after attempt", []() {
		data::GpsData gps;
		struct ExtGPSPosition pos1;
		pos1.position.altitude = 1000;
		struct ExtGPSPosition pos2;
		pos2.position.altitude = 2000;
		gps.setBasePos(1, pos1);
		assert(gps.getBaseAlt(), 1000, helper::eqi);
		gps.setBasePos(0, pos2);
		assert(gps.getBaseAlt(), 1000, helper::eqi);
		gps.setBasePos(0, pos2);
		assert(gps.getBaseAlt(), 2000, helper::eqi);
	});

	describe<data::SensorData>("sensoric data", runner)->test("extract WIMWV",
			[]()
			{
				assert(helper::pars_wind.unpack("$WIMWV,242.8,R,6.9,N,A*20\r", 0), MSG_UNPACK_SUC, helper::eqi);
				assert(vfrb::VFRB::msSensorData.getMwvStr(), std::string("$WIMWV,242.8,R,6.9,N,A*20\r\n"), helper::eqs);
				assert(vfrb::VFRB::msSensorData.getMwvStr(), std::string(""), helper::eqs);
			})->test("extract WIMDA",
			[]() {
				vfrb::VFRB::msSensorData.setMdaStr(0, "$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E\r");
				assert(vfrb::VFRB::msSensorData.getMdaStr(), std::string("$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E\r\n"), helper::eqs);
				assert(vfrb::VFRB::msSensorData.getMdaStr(), std::string(""), helper::eqs);
			})->test("write higher priority", []() {
		vfrb::VFRB::msSensorData.setPress(0, 900.0);
		assert(vfrb::VFRB::msSensorData.getPress(), 900.0, helper::eqd);
		vfrb::VFRB::msSensorData.setPress(1, 950.0);
		assert(vfrb::VFRB::msSensorData.getPress(), 950.0, helper::eqd);
		vfrb::VFRB::msSensorData.setPress(0, 900.0);
		assert(vfrb::VFRB::msSensorData.getPress(), 950.0, helper::eqd);
	})->test("write after attempt", []() {
		vfrb::VFRB::msSensorData.setPress(2, 900.0);
		assert(vfrb::VFRB::msSensorData.getPress(), 900.0, helper::eqd);
		vfrb::VFRB::msSensorData.setPress(0, 950.0);
		assert(vfrb::VFRB::msSensorData.getPress(), 900.0, helper::eqd);
		vfrb::VFRB::msSensorData.setPress(0, 950.0);
		assert(vfrb::VFRB::msSensorData.getPress(), 950.0, helper::eqd);
	});
}
