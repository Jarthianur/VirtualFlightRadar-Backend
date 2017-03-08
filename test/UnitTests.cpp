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

#include "../src/aircraft/AircraftProcessor.h"
#include "../src/config/Configuration.h"
#include "../src/data/ClimateData.h"
#include "../src/parser/APRSParser.h"
#include "../src/parser/SBSParser.h"
#include "../src/util/GPSmodule.h"
#include "Assertion.hpp"

using namespace assertion;

/**
 * testing methods;
 */

void testParsSbsUnpack(SBSParser& pars_sbs)
{
    //correct msg
    std::string msg(
            "MSG,3,0,0,AAAAAA,0,2016/01/01,10:00:00.000,2016/01/01,10:00:00.000,,1000,,,1.0,1.0,,,,,,0");
    ASSERT(pars_sbs.unpack(msg), 0, msg);

    //incorrect msg
    msg =
            "MSG,3,0,0,AAAAAA,0,2016/01/01,10:00:00.000,2016/01/01,10:00:00.000,,1000,,,,,,,,,,0";
    ASSERT(pars_sbs.unpack(msg), -2, msg);

    //corrupted msg
    msg =
            "MSG,3,0,0,AA,AAA,A,0,2016/01sdsd/01,10:00:00.000343,20asx16/01/01,10:0,0:00,.000,,1000,,,1.0,1.0,,,,,,0";
    ASSERT(pars_sbs.unpack(msg), -1, msg);

    return;
}

/*void testParsAprsUnpack(APRSParser& pars_aprs, AircraftContainer& ac_cont)
{
    //correct msg
    std::string msg(
            "ICAAAAAAA>APRS,qAS,EDDD:/100000h0001.00N/00001.00E'100/100/A=001000 !W58! id09AAAAAA -100fpm +1.0rot 17.8dB 0e -13.0kHz gps11x17");
    if (pars_aprs.unpack(std::ref(msg), std::ref(ac_cont)) != 0)
        fail("'unpack correct aprs msg'");
    tests++;

    //incorrect msg
    msg =
            "Musterstadt>APRS,TCPIP*,qAC,GLIDERN1:/100000h0001.00NI00001.00E&000/000/A=001000 v0.2.3.RPI-GPU CPU:1.0 RAM:150.2/455.7MB NTP:1.7ms/-6.4ppm +43.8C RF:-0.36dB";
    if (pars_aprs.unpack(std::ref(msg), std::ref(ac_cont)) == 0)
        fail("'unpack incorrect aprs msg'");
    tests++;

    //corrupted msg
    msg =
            "Po342>eim>APRS,TCPIP*,qAC,GLIDsdgrERN1:/105442h5031.96NI00843.66E&dw000/a/A=000774 v0.2.3.RxadwxPI-GPU CPU:1.0 RA1wM:150.2/455.7MB NTP:1.7ms/-6.4ppm +43.8C RF:-0.36dB";
    if (pars_aprs.unpack(std::ref(msg), std::ref(ac_cont)) == 0)
        fail("'unpack corrupted aprs msg'");
    tests++;
    return;
}*/

void testAcProcProcess(AircraftProcessor& ac_proc)
{
    return;
}
void testAcProcChecksum(AircraftProcessor& ac_proc)
{
    return;
}
void testAcProcGpsFix(AircraftProcessor& ac_proc)
{
    return;
}

void testWFWriteNMEA(ClimateData& w_feed)
{
    return;
}
void testWFIsValid(ClimateData& w_feed)
{
    return;
}
void testWFGetPress(ClimateData& w_feed)
{
    return;
}
void testWFGetTemp(ClimateData& w_feed)
{
    return;
}
void testWFGetNMEA(ClimateData& w_feed)
{
    return;
}
void testWFIsNotValidAfterGet(ClimateData& w_feed)
{
    return;
}

int main(int argc, char* argv[])
{

    //setup Configuration
    Configuration::base_latitude = 0.0;
    Configuration::base_longitude = 0.0;
    Configuration::base_altitude = 0;
    Configuration::base_geoid = 0.0;
    Configuration::filter_maxDist = 100000000;
    Configuration::filter_maxHeight = 1000000;

    AircraftProcessor ac_proc(Configuration::base_latitude, Configuration::base_longitude,
                              Configuration::base_altitude);
    GPSmodule gpsm(Configuration::base_latitude, Configuration::base_longitude,
                   Configuration::base_altitude, Configuration::base_geoid);
    SBSParser pars_sbs;
    APRSParser pars_aprs;
    ClimateData w_feed;

    //run tests
    std::cout << "=== RUNNING UNIT TESTS ===" << std::endl;
    testParsSbsUnpack(std::ref(pars_sbs));
    /*testParsAprsUnpack(std::ref(pars_aprs));

    testAcProcProcess(std::ref(ac_proc));
    testAcProcChecksum(std::ref(ac_proc));
    testAcProcGpsFix(std::ref(ac_proc));

    testAcContProcessAircraft(std::ref(ac_cont));
    testAcContInvalidateAircrafts(std::ref(ac_cont));
    testAcContGetContSize(std::ref(ac_cont));
    testAcContClear(std::ref(ac_cont));

    testWFWriteNMEA(std::ref(w_feed));
    testWFIsValid(std::ref(w_feed));
    testWFGetPress(std::ref(w_feed));
    testWFGetTemp(std::ref(w_feed));
    testWFGetNMEA(std::ref(w_feed));
    testWFIsNotValidAfterGet(std::ref(w_feed));*/

    std::cout << "=== TEST FINISHED ===" << std::endl;
    PRINT_RESULT();
    return NUM_OF_FAILS;
}

