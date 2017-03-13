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

#include <iostream>
#include <string>

#include "../src/parser/SBSParser.h"
#include "framework/src/framework.h"
#include "../src/vfrb/VFRB.h"

using namespace testsuite;

#ifdef assert
#undef assert
#endif

int main(int argc, char* argv[])
{
    auto rep = reporter::createXmlReporter(std::cout);

    SBSParser pars_sbs;

    std::string msg1(
            "MSG,3,0,0,AAAAAA,0,2016/01/01,10:00:00.000,2016/01/01,10:00:00.000,,1000,,,1.0,1.0,,,,,,0");
    std::string msg2(
            "MSG,3,0,0,AAAAAA,0,2016/01/01,10:00:00.000,2016/01/01,10:00:00.000,,1000,,,,,,,,,,0");
    std::string msg3(
            "MSG,3,0,0,AA,AAA,A,0,2016/01sdsd/01,10:00:00.000343,20asx16/01/01,10:0,0:00,.000,,1000,,,1.0,1.0,,,,,,0");

    test("SBS parser - unpack", rep)->assert("valid msg", &SBSParser::unpack, pars_sbs, 0, comparator::EQUALS<int>(), msg1);

    return rep->report();
}

