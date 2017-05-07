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

#include "../../src/config/ConfigReader.h"
#include "../../src/config/Configuration.h"
#include "../framework/src/comparator/Comparators.hpp"
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

void test_config(TestSuitesRunner& runner)
{
    describe<config::ConfigReader>("read config", runner)->test("read", []()
    {
        config::ConfigReader cr("test.ini"); // depends on $PWD
            cr.read();
            assert(cr.getProperty(SECT_KEY_FALLBACK, KV_KEY_LATITUDE, "invalid"), std::string("0.000000"), helper::eqs);
            assert(cr.getProperty(SECT_KEY_FALLBACK, KV_KEY_LONGITUDE, "invalid"), std::string("invalid"), helper::eqs);
            assert(cr.getProperty(SECT_KEY_FALLBACK, KV_KEY_ALTITUDE, "invalid"), std::string("1000"), helper::eqs);
            assert(cr.getProperty(SECT_KEY_FALLBACK, KV_KEY_GEOID, "invalid"), std::string("invalid"), helper::eqs);
            assert(cr.getProperty("nothing", ""), std::string(""), helper::eqs);
        });
}
