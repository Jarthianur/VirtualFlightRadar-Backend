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

#ifndef TEST_HELPER_HPP_
#define TEST_HELPER_HPP_

#include <string>
#include <boost/regex.hpp>

#include "../src/config/Configuration.h"
#include "../src/data/AircraftContainer.h"
#include "../src/data/GPSData.h"
#include "../src/data/SensorData.h"
#include "../src/parser/APRSParser.h"
#include "../src/parser/GPSParser.h"
#include "../src/parser/SBSParser.h"
#include "../src/parser/SensorParser.h"
#include "../src/util/Priority.h"
#include "../src/vfrb/VFRB.h"
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
        vfrb::VFRB::msAcCont.processAircrafts();
    }
}

/**
 * set vfrb statics to config
 */
inline void setupVFRB()
{
    vfrb::VFRB::msSensorData.setPress(util::Priority::DONTCARE,
            config::Configuration::base_pressure);
    vfrb::VFRB::msGPSdata.setDefaults(config::Configuration::base_latitude,
            config::Configuration::base_longitude,
            config::Configuration::base_altitude,
            config::Configuration::base_geoid);
}

static testsuite::comparator::Comparator<int> eqi =
        testsuite::comparator::EQUALS<int>();
static testsuite::comparator::Comparator<double> eqd =
        testsuite::comparator::EQUALS<double>();
static testsuite::comparator::Comparator<std::string> eqs =
        testsuite::comparator::EQUALS<std::string>();
static testsuite::comparator::Comparator<bool> eqb =
        testsuite::comparator::EQUALS<bool>();

static parser::SBSParser pars_sbs;
static parser::APRSParser pars_aprs;
static parser::SensorParser pars_wind;
static parser::GPSParser pars_gps;

static boost::regex pflauRE(
        "\\$PFLAU,,,,1,0,([-]?\\d+?),0,(\\d+?),(\\d+?),(\\S{6})\\*(?:\\S{2})");
static boost::regex pflaaRE(
        "\\$PFLAA,0,([-]?\\d+?),([-]?\\d+?),([-]?\\d+?),(\\d+?),(\\S{6}),(\\d{3})?,,(\\d+?)?,([-]?\\d+?\\.\\d+?)?,([0-9A-F])\\*(?:\\S{2})");

} // helper

#endif /* TEST_HELPER_HPP_ */
