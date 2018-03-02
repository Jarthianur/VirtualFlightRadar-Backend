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
#include <fstream>
#include <stdexcept>

#include "config/Configuration.h"
#include "Logger.h"
#include "VFRB.h"

#ifndef VERSION
#define VERSION "DEMO"
#endif

/**
 * @fn evalArgs
 * @brief Evaluate comandline arguments.
 * @param argc The argument count
 * @param argv The arguments
 * @return 0 if succeeded, -1 in case of failure
 */
config::Configuration evalArgs(std::int32_t argc, char** argv);

/**
 * @fn main
 * @brief The application start point.
 * @param argc The argument count
 * @param argv The arguments
 * @return 0 on success, else -1
 */
int main(int argc, char** argv)
{
    Logger::info({"VirtualFlightRadar-Backend -- " VERSION});

    if(argc < 3)
    {
        Logger::info(
            {"usage: ./VirtualFlightRadar-Backend [OPTIONS] [-c | --config] path_to_file.ini\n"
             "Run VFR-B with given config file.\n"
             "The config file must be in valid '.ini' format!\n"
             "OPTIONS:\n"
             "-g | --gnd-mode : Force ground-mode, GPS feed will stop if a 'good' position is received."});
        return -1;
    }

    try
    {
        VFRB vfrb(evalArgs(argc, argv));
        vfrb.run();
    }
    catch(const std::exception& e)
    {
        Logger::error({"(VFRB) fatal: ", e.what()});
        return -1;
    }

    return 0;
}

config::Configuration evalArgs(std::int32_t argc, char** argv)
{
    std::string ini_file;
    bool gnd       = false;
    bool cfg_found = false;

    for(int i = 1; i < argc; i++)
    {
        if(std::string(argv[i]).find("-c") != std::string::npos && i + 1 < argc)
        {
            ini_file = std::string(argv[++i]);
            if(!(cfg_found = ini_file.rfind(".ini") != std::string::npos))
            {
                throw std::runtime_error(ini_file + " is not a .ini file.");
            }
        }
        else if(std::string(argv[i]).find("-g") != std::string::npos)
        {
            gnd = true;
        }
        else
        {
            Logger::warn({"(VFRB) unrecognized option: ", std::string(argv[i])});
        }
    }

    if(cfg_found)
    {
        std::ifstream file(ini_file);
        config::Configuration conf(file);
        if(gnd)
        {
            conf.forceGndMode();
            Logger::info({"(VFRB) Override ground mode: Yes"});
        }
        return conf;
    }
    else
    {
        throw std::runtime_error("No config file given.");
    }
}
