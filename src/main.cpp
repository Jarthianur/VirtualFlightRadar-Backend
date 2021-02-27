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

#include "config/CConfiguration.hpp"

#include "CLogger.hpp"
#include "CProgramOptions.hpp"
#include "CVfrb.hpp"

using namespace vfrb;
using namespace vfrb::config;
using namespace std::literals;

static auto& logger = CLogger::Instance();

auto
getConfig(usize argc_, str* argv_) -> Shared<CConfiguration> {
    CProgramOptions opts;
    opts.Parse(argc_, argv_);
    auto confFile = opts.RequireOpt(CProgramOptions::OPT_KEY_CONF);
    auto logFile  = opts.GetOpt(CProgramOptions::OPT_KEY_LOGF);
    auto gndMode  = opts.GetOpt(CProgramOptions::OPT_KEY_GNDM);
    if (logFile) {
        logger.LogFile(*logFile);
    }
    std::ifstream confStream(*confFile);
    auto          conf = Share<CConfiguration>(confStream);
    if (gndMode) {
        conf->GroundMode = true;
        logger.Info("(VFRB) Force ground mode");
    }
    return conf;
}

auto
main(i32 argc_, str* argv_) -> i32 {
    try {
        CVfrb vfrb(getConfig(static_cast<usize>(argc_), argv_));
        vfrb.Run();
    } catch ([[maybe_unused]] vfrb::error::COptsCalledForHelp const&) {
        return 1;
    } catch (vfrb::error::IError const& e) {
        logger.Error("(VFRB) fatal: ", e.Message());
        return 1;
    }
    return 0;
}
