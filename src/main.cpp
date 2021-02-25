/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>

#include "config/CConfiguration.hpp"
#include "error/IError.hpp"

#include "CLogger.hpp"
#include "CVfrb.hpp"

#ifndef VERSION
constexpr auto VERSION = "DEMO";
#endif

using namespace vfrb;
using namespace vfrb::config;
using namespace boost;
using namespace std::literals;

static auto& logger = CLogger::Instance();

namespace error
{
class CConfigFileError : public vfrb::error::IError
{
    String const m_msg;

public:
    explicit CConfigFileError(String const& msg_) : m_msg(msg_) {}

    str
    Message() const noexcept override {
        return m_msg.c_str();
    }
};

class CArgumentError : public vfrb::error::IError
{
public:
    CArgumentError() = default;

    str
    Message() const noexcept override {
        return "";
    }
};
}  // namespace error

program_options::variables_map
evalArgs(int argc_, char** argv_) {
    program_options::options_description cmdline_options("VirtualFlightRadar-Backend -- "s + VERSION);
    cmdline_options.add_options()("help,h", "show this message");
    cmdline_options.add_options()(
        "ground-mode,g", "forcibly enable ground mode; GPS feeds will stop when a good position is received");
    cmdline_options.add_options()("config,c", program_options::value<std::string>(), "config file");
    cmdline_options.add_options()("output,o", program_options::value<std::string>(), "specify where to log");
    program_options::variables_map variables;

    try {
        program_options::store(program_options::parse_command_line(argc_, argv_, cmdline_options), variables);
        program_options::notify(variables);
    } catch ([[maybe_unused]] boost::exception const&) {
        std::cout << cmdline_options << std::endl;
        throw ::error::CArgumentError();
    }

    if (argc_ < 3 || variables.count("help")) {
        std::cout << cmdline_options << std::endl;
        throw ::error::CArgumentError();
    }
    return variables;
}

SPtr<CConfiguration>
getConfig(program_options::variables_map const& vars_) {
    if (vars_.count("output")) {
        logger.LogFile(vars_["output"].as<std::string>());
    }
    logger.Info("VirtualFlightRadar-Backend -- ", VERSION);
    if (vars_.count("config")) {
        std::ifstream file(vars_["config"].as<std::string>());
        if (!file) {
            throw ::error::CConfigFileError(vars_["config"].as<std::string>() + " is not accessible");
        }
        auto conf = std::make_shared<CConfiguration>(file);
        if (vars_.count("ground-mode")) {
            conf->GroundMode = true;
            logger.Info("(VFRB) Override ground mode: Yes");
        }
        return conf;
    } else {
        throw ::error::CConfigFileError("No config file given.");
    }
}

/**
 * @fn main
 * @brief The application start point.
 * @param argc The argument count
 * @param argv The arguments
 * @return 0 on success, else -1
 */
int
main(int argc_, char** argv_) {
    try {
        CVfrb vfrb(getConfig(evalArgs(argc_, argv_)));
        vfrb.Run();
    } catch ([[maybe_unused]] ::error::CArgumentError const&) {
        return 1;
    } catch (vfrb::error::IError const& e) {
        logger.Error("(VFRB) fatal: ", e.Message());
        return 1;
    }
    return 0;
}
