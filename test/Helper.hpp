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

#ifndef TEST_HELPER_HPP_
#define TEST_HELPER_HPP_

#include <boost/regex.hpp>
#include <string>

#include "../src/aircraft/Aircraft.hpp"
#include "../src/aircraft/AircraftContainer.h"
#include "../src/config/Configuration.h"
#include "../src/data/AtmosphereData.h"
#include "../src/data/GpsData.h"
#include "../src/parser/AprsParser.h"
#include "../src/parser/GpsParser.h"
#include "../src/parser/SbsParser.h"
#include "../src/parser/SensorParser.h"
#include "../src/util/Position.h"
#include "../src/util/SensorInfo.h"
#include "../src/VFRB.h"
#include "framework/src/comparator/Comparators.hpp"
#include "framework/src/comparator/ComparatorStrategy.hpp"

namespace helper
{
/**
 * after that, inserted aircraft is not reported until update
 */
inline void clearAcCont()
{
	for (int i = 0; i < 4; ++i)
	{
		VFRB::msAcCont.processAircrafts();
	}
}

/**
 * set vfrb statics to config
 */
inline void setupVFRB()
{
	VFRB::msSensorData.init( { "", "", config::Configuration::base_pressure });
	VFRB::msGpsData.init( { { config::Configuration::base_latitude,
	        config::Configuration::base_longitude, config::Configuration::base_altitude },
	        1, 5, config::Configuration::base_geoid, 0.0 });
}

static testsuite::comparator::Comparator<int> eqi = testsuite::comparator::EQUALS<int>();
static testsuite::comparator::Comparator<double> eqd = testsuite::comparator::EQUALS<
        double>();
static testsuite::comparator::Comparator<std::string> eqs = testsuite::comparator::EQUALS<
        std::string>();
static testsuite::comparator::Comparator<bool> eqb =
        testsuite::comparator::EQUALS<bool>();

static parser::SbsParser parsSbs;
static parser::AprsParser parsAprs;
static parser::SensorParser parsSens;
static parser::GpsParser parsGps;

static boost::regex pflauRe(
        "\\$PFLAU,,,,1,0,([-]?\\d+?),0,(\\d+?),(\\d+?),(\\S{6})\\*(?:\\S{2})");
static boost::regex pflaaRe(
        "\\$PFLAA,0,([-]?\\d+?),([-]?\\d+?),([-]?\\d+?),(\\d+?),(\\S{6}),(\\d{3})?,,(\\d+?)?,([-]?\\d+?\\.\\d+?)?,([0-9A-F])\\*(?:\\S{2})");
static boost::regex gpsRe(
        "(\\$GPRMC,\\d{6},A,0000\\.00,N,00000\\.00,E,0,0,\\d{6},001\\.0,W\\*[0-9a-fA-F]{2}\\s*)?(\\$GPGGA,\\d{6},0000\\.0000,N,00000\\.0000,E,1,00,1,0,M,0\\.0,M,,\\*[0-9a-fA-F]{2}\\s*)?");

} // helper

#endif /* TEST_HELPER_HPP_ */
