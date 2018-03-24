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

#include "framework/src/framework.h"
#include "Helper.hpp"

using namespace testsuite;

#ifdef assert
#undef assert
#endif

TEST_FUNCTION(test_config)
TEST_FUNCTION(test_data)
TEST_FUNCTION(test_data_processor)
TEST_FUNCTION(test_feed_parser)
TEST_FUNCTION(test_object)
TEST_FUNCTION(test_math)

int main(int, char**)
{
    auto rep = reporter::createXmlReporter();
    TestSuitesRunner runner;

    test_config(runner);
    test_data(runner);
    test_data_processor(runner);
    test_feed_parser(runner);
    test_object(runner);
    test_math(runner);

    return rep->report(runner);
}
