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

#include <boost/program_options.hpp>

#include "config/Configuration.h"
#include "error/Error.hpp"
#include "util/Logger.hpp"

#include "VFRB.h"

#ifndef VERSION
constexpr auto VERSION = "DEMO";
#endif

using namespace vfrb;
using namespace vfrb::config;
using namespace boost;
using namespace std::literals;

static auto& logger = Logger::instance();

namespace error
{
class ConfigFileError : public vfrb::error::Error
{
    str const m_msg;

public:
    explicit ConfigFileError(str const& msg) : m_msg(msg) {}
    ~ConfigFileError() noexcept override = default;

    char const* what() const noexcept override
    {
        return m_msg.c_str();
    }
};

class ArgumentError : public vfrb::error::Error
{
public:
    ArgumentError()                    = default;
    ~ArgumentError() noexcept override = default;

    char const* what() const noexcept override
    {
        return "";
    }
};
}  // namespace error

program_options::variables_map evalArgs(int argc, char** argv)
{
    program_options::options_description cmdline_options("VirtualFlightRadar-Backend -- "s + VERSION);
    cmdline_options.add_options()("help,h", "show this message");
    cmdline_options.add_options()("verbose,v", "enable debug logging");
    cmdline_options.add_options()(
        "ground-mode,g", "forcibly enable ground mode; GPS feeds will stop when a good position is received");
    cmdline_options.add_options()("config,c", program_options::value<std::string>(), "config file");
    cmdline_options.add_options()("output,o", program_options::value<std::string>(), "specify where to log");
    program_options::variables_map variables;
    program_options::store(program_options::parse_command_line(argc, argv, cmdline_options), variables);
    program_options::notify(variables);

    if (argc < 3 || variables.count("help"))
    {
        std::cout << cmdline_options << std::endl;
        throw ::error::ArgumentError();
    }
    return variables;
}

s_ptr<Configuration> get_config(const program_options::variables_map& variables)
{
    if (variables.count("verbose"))
    {
        logger.debug();
    }
    if (variables.count("output"))
    {
        logger.logFile(variables["output"].as<std::string>());
    }
    logger.info("VirtualFlightRadar-Backend -- " VERSION);
    if (variables.count("config"))
    {
        std::ifstream file(variables["config"].as<std::string>());
        if (!file)
        {
            throw ::error::ConfigFileError(variables["config"].as<std::string>() + " is not accessible");
        }
        auto conf = std::make_shared<Configuration>(file);
        if (variables.count("ground-mode"))
        {
            conf->groundMode = true;
            logger.info("(VFRB) Override ground mode: Yes");
        }
        return conf;
    }
    else
    {
        throw ::error::ConfigFileError("No config file given.");
    }
}

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
    catch ([[maybe_unused]] ::error::ArgumentError const&)
    {
        return 1;
    }
    catch (vfrb::error::Error const& e)
    {
        logger.error("(VFRB) fatal: ", e.what());
        return 1;
    }
    return 0;
}
