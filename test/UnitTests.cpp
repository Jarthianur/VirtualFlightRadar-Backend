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

#include <cstdint>

#include "../src/config/Configuration.h"
#include "framework/src/framework.h"

using namespace testsuite;

#ifdef assert
#undef assert
#endif

extern void test_util(TestSuitesRunner&);
extern void test_config(TestSuitesRunner&);
extern void test_data(TestSuitesRunner&);
extern void test_aircraft(TestSuitesRunner&);
extern void test_parser(TestSuitesRunner&);

int main(int argc, char *argv[])
{
    auto rep = reporter::createXmlReporter();
    TestSuitesRunner runner;

    config::Configuration::filter_maxHeight = INT32_MAX;
    config::Configuration::filter_maxDist = INT32_MAX;

    test_util(runner);
    test_config(runner);
    test_aircraft(runner);
    test_data(runner);
    test_parser(runner);

    return rep->report(runner);
}
