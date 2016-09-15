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

#include <iostream>
#include "../src/Math.h"
#include "../src/Aircraft.h"
#include "../src/AircraftContainer.h"
#include "../src/AircraftProcessor.h"
#include "../src/ParserAPRS.h"
#include "../src/ParserSBS.h"
#include "../src/Configuration.h"
#include "../src/WeatherFeed.h"

/**
 * testing methods;
 * returning +1 on success, 0 on failure
 */
int testParsSbsUnpack(std::ref(pars_sbs))
{
    return 1;
}
int testParsAprsUnpack(std::ref(pars_aprs))
{
    return 1;
}

int testAcProcProcess(std::ref(ac_proc))
{
    return 1;
}
int testAcProcChecksum(std::ref(ac_proc))
{
    return 1;
}
int testAcProcGpsFix(std::ref(ac_proc))
{
    return 1;
}

int testAcContProcessAircraft(std::ref(ac_cont))
{
    return 1;
}
int testAcContInvalidateAircrafts(std::ref(ac_cont))
{
    return 1;
}
int testAcContGetContSize(std::ref(ac_cont))
{
    return 1;
}
int testAcContClear(std::ref(ac_cont))
{
    return 1;
}

int testWFWriteNMEA(std::ref(w_feed))
{
    return 1;
}
int testWFIsValid(std::ref(w_feed))
{
    return 1;
}
int testWFGetPress(std::ref(w_feed))
{
    return 1;
}
int testWFGetTemp(std::ref(w_feed))
{
    return 1;
}
int testWFGetNMEA(std::ref(w_feed))
{
    return 1;
}
int testWFIsNotValidAfterGet(std::ref(w_feed))
{
    return 1;
}

int main(int argc, char* argv[]) {

    //setup Configuration
    Configuration::base_latitude = 0.0;
    Configuration::base_longitude = 0.0;
    Configuration::base_altitude = 0;
    Configuration::base_geoid = 0.0;

    //init objects
    AircraftContainer ac_cont;
    AircraftProcessor ac_proc(Configuration::base_latitude,
            Configuration::base_longitude, Configuration::base_altitude,
            Configuration::base_geoid);
    ParserSBS pars_sbs;
    ParserAPRS pars_aprs;
    WeatherFeed w_feed;

    //run tests
    std::cout << "=== RUNNING UNIT TESTS ===" << std::endl;
    int suc = 0;
    suc += testParsSbsUnpack(std::ref(pars_sbs));
    suc += testParsAprsUnpack(std::ref(pars_aprs));

    suc += testAcProcProcess(std::ref(ac_proc));
    suc += testAcProcChecksum(std::ref(ac_proc));
    suc += testAcProcGpsFix(std::ref(ac_proc));

    suc += testAcContProcessAircraft(std::ref(ac_cont));
    suc += testAcContInvalidateAircrafts(std::ref(ac_cont));
    suc += testAcContGetContSize(std::ref(ac_cont));
    suc += testAcContClear(std::ref(ac_cont));

    suc += testWFWriteNMEA(std::ref(w_feed));
    suc += testWFIsValid(std::ref(w_feed));
    suc += testWFGetPress(std::ref(w_feed));
    suc += testWFGetTemp(std::ref(w_feed));
    suc += testWFGetNMEA(std::ref(w_feed));
    suc += testWFIsNotValidAfterGet(std::ref(w_feed));

    std::cout << "=== TEST FINISHED ===" << std::endl;
    std::cout << "passed: " << suc << "/15" << " ; failures: " << 15-suc << "/15" << std::endl;
    return 15-suc;
}

