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
#include <fstream>

#include "config/Configuration.h"
#include "data/SensorData.h"
#include "data/GpsData.h"
#include "util/Logger.h"
#include "VFRB.h"

#ifndef VERSION
#define VERSION "DEMO"
#endif

using namespace util;

/**
 * Evaluate comandline arguments.
 *
 * @param argc the argument count
 * @param argv the arguments
 *
 * @return 0 if succeeded, -1 in case of failure
 */
std::int32_t evalArgs(std::int32_t /*argc*/, char** /*argv*/);

/**
 * The application start point.
 * In here the commandline arguments get evaluated,
 * the Configuration initialized and the VFRB ran.
 */
int main(int argc, char** argv)
{
    Logger::info("VirtualFlightRadar-Backend -- ", VERSION);

    if (argc < 3)
    {
        Logger::info(
                "usage: ./VirtualFlightRadar-Backend [OPTIONS] [-c | --config] path_to_file.ini\n"
                        "Run VFR-B with given config file.\n"
                        "The config file must be in valid '.ini' format!\n"
                        "OPTIONS:\n"
                        "-g | --gnd-mode : Force ground-mode, GPS feed will stop if a 'good' position is received.");
        return -1;
    }

    if (evalArgs(argc, argv) != 0)
    {
        return -1;
    }

    // set climate fallbacks
    VFRB::msSensorData.setPress(0, config::Configuration::base_pressure);
    VFRB::msGpsData.setDefaults(config::Configuration::base_latitude,
            config::Configuration::base_longitude,
            config::Configuration::base_altitude,
            config::Configuration::base_geoid);
    VFRB::run();

    return 0;
}

std::int32_t evalArgs(std::int32_t argc, char** argv)
{
    std::string ini_file;
    bool gnd = false;
    bool cfg_found = false;

    for (int i = 1; i < argc; i++)
    {
        if (std::string(argv[i]).find("-c") != std::string::npos && i + 1
                < argc)
        {
            ini_file = std::string(argv[++i]);
            if (ini_file.rfind(".ini") == std::string::npos)
            {
                Logger::error("(VFRB) not a ini file: ", ini_file);
                return -1;
            } else
            {
                cfg_found = true;
            }
        } else if (std::string(argv[i]).find("-g") != std::string::npos)
        {
            gnd = true;
        } else
        {
            Logger::warn("(VFRB) unrecognized option: ", std::string(argv[i]));
        }
    }

    if (cfg_found)
    {
        try
        {
            std::ifstream file(ini_file);
            config::Configuration conf(file);
        } catch (const std::logic_error& e)
        {
            Logger::error("(VFRB) eval config: ", e.what());
            return -1;
        }
    } else
    {
        Logger::error("(VFRB) no config file given");
        return -1;
    }

    if (gnd || config::Configuration::global_gnd_mode)
    {
        config::Configuration::global_gnd_mode = true;
        Logger::info("(VFRB) GND mode: yes");
    } else
    {
        Logger::info("(VFRB) GND mode: no");
    }

    return 0;
}

