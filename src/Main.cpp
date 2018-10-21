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

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <boost/program_options.hpp>

#include "config/Configuration.h"
#include "util/Logger.hpp"

#include "VFRB.h"

#ifndef VERSION
#    define VERSION "DEMO"
#endif

using namespace config;
using namespace boost;

program_options::variables_map evalArgs(int argc, char** argv);

std::shared_ptr<Configuration> get_config(const program_options::variables_map& variables);

/**
 * @fn main
 * @brief The application start point.
 * @param argc The argument count
 * @param argv The arguments
 * @return 0 on success, else -1
 */
int main(int argc, char** argv)
{
    try
    {
        VFRB vfrb(get_config(evalArgs(argc, argv)));
        vfrb.run();
    }
    catch (const std::exception& e)
    {
        logger.error("(VFRB) fatal: ", e.what());
        return 1;
    }
    catch (...)
    {
        return 1;
    }
    return 0;
}

program_options::variables_map evalArgs(int argc, char** argv)
{
    program_options::options_description cmdline_options("VirtualFlightRadar-Backend -- " VERSION);
    cmdline_options.add_options()("help,h", "show this message");
    cmdline_options.add_options()("verbose,v", "enable debug logging");
    cmdline_options.add_options()(
        "ground-mode,g",
        "forcibly enable ground mode; GPS feeds will stop when a good position is received");
    cmdline_options.add_options()("config,c", program_options::value<std::string>(), "config file");
    cmdline_options.add_options()("output,o", program_options::value<std::string>(),
                                  "specify where to log");
    program_options::variables_map variables;
    program_options::store(program_options::parse_command_line(argc, argv, cmdline_options),
                           variables);
    program_options::notify(variables);

    if (argc < 3 || variables.count("help"))
    {
        std::cout << cmdline_options << std::endl;
        throw 1;
    }
    return variables;
}

std::shared_ptr<Configuration> get_config(const program_options::variables_map& variables)
{
    if (variables.count("verbose"))
    {
        logger.set_debug();
    }
    if (variables.count("output"))
    {
        logger.set_logFile(variables["output"].as<std::string>());
    }
    logger.info("VirtualFlightRadar-Backend -- " VERSION);
    if (variables.count("config"))
    {
        std::ifstream file(variables["config"].as<std::string>());
        if (!file)
        {
            throw std::runtime_error(variables["config"].as<std::string>() + " is not accessible");
        }
        auto conf = std::make_shared<Configuration>(file);
        if (variables.count("ground-mode"))
        {
            conf->set_groundMode(true);
            logger.info("(VFRB) Override ground mode: Yes");
        }
        return conf;
    }
    else
    {
        throw std::runtime_error("No config file given.");
    }
}
