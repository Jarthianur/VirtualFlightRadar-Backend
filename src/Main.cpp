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

#include <stdexcept>
#include <string>

#include "config/Configuration.h"
#include "data/SensorData.h"
#include "data/GPSData.h"
#include "util/Logger.h"
#include "util/Priority.h"
#include "vfrb/VFRB.h"

#ifndef VERSION
#define VERSION "DEMO"
#endif

int main(int argc, char* argv[])
{
    if (argc != 3 || std::string(argv[1]) != "-c"
        || std::string(argv[2]).rfind(".ini") == std::string::npos)
    {
        Logger::info("usage: ./VirtualFlightRadar-Backend -c path_to_file.ini");
        return -1;
    }
    Logger::info("VirtualFlightRadar-Backend -- ", VERSION);

    try
    {
        Configuration conf(argv[2]);
    }
    catch (const std::logic_error& e)
    {
        Logger::error("(VFRB) eval config: ", e.what());
        return -1;
    }

    // set climate fallbacks
    VFRB::msSensorData.setPress(Priority::DONTCARE, Configuration::base_pressure);
    VFRB::msGPSdata.setDefaults(Configuration::base_latitude,
                                Configuration::base_longitude,
                                Configuration::base_altitude, Configuration::base_geoid);
    VFRB::run();

    return 0;
}

