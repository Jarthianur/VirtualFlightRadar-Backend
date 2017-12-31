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

#include "../../src/util/GpsModule.h"
#include "../../src/util/Math.hpp"
#include "../../src/util/Position.h"
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

void test_util(TestSuitesRunner& runner)
{
	describe("math utils", runner, "math")->test("radian", []()
	{
		assert(math::radian(45.0), 0.785398, helper::eqd);
		assert(math::radian(0.0), 0.0, helper::eqd);
		assert(math::radian(360.0), 6.28319, helper::eqd);
	})->test("degree", []()
	{
		assert(math::degree(0.785398), 45.0, helper::eqd);
		assert(math::degree(0.0), 0.0, helper::eqd);
		assert(math::degree(6.28319), 360.0, helper::eqd);
	})->test("dToI", []()
	{
		assert(math::dToI(0.0), 0, helper::eqi);
		assert(math::dToI(1.4), 1, helper::eqi);
		assert(math::dToI(1.5), 2, helper::eqi);
		assert(math::dToI(-1.4), -1, helper::eqi);
		assert(math::dToI(-1.5), -2, helper::eqi);
	})->test("dmToDeg", []()
	{
		assert(math::dmToDeg(0.0), 0.0, helper::eqd);
		assert(math::dmToDeg(9030.50), 90.508333, helper::eqd);
		assert(math::dmToDeg(18000.0), 180.0, helper::eqd);
		assert(math::dmToDeg(-4512.3456), 45.205760, helper::eqd);
	})->test("calcIcaoHeight", []()
	{
		assert(math::calcIcaoHeight(0.0), 44330.769231, helper::eqd);
		assert(math::calcIcaoHeight(1013.25), 0.0, helper::eqd);
		assert(math::calcIcaoHeight(980.0), 280.578763, helper::eqd);
	})->test("checksum", []()
	{
		assert(math::checksum("", sizeof("")), 0, helper::eqi);
		assert(math::checksum("\0", sizeof("\0")), 0, helper::eqi);
		assert(math::checksum("$abc*", sizeof("$abc*")), 96, helper::eqi);
	});

	describe<GpsModule>("GPSModule - gpsfix", runner)->test("gps nmea", []()
	{
		GpsModule gpsm;
		boost::smatch match;
		struct ExtGpsPosition pos;
		bool matched = boost::regex_search(gpsm.genGpggaStr(pos), match, helper::gpsRe);
		assert(matched, true, helper::eqb);
		matched = boost::regex_search(gpsm.genGprmcStr(pos), match, helper::gpsRe);
		assert(matched, true, helper::eqb);
	});
}
