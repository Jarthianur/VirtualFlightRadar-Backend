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

#include "CProgramOptions.hpp"

#include <iostream>

namespace vfrb
{
#ifndef VFRB_VERSION
CTCONST VFRB_VERSION = "DEMO";
#endif

namespace error
{
CMissingArgumentError::CMissingArgumentError(String const& arg_) : m_msg("Argument missing for " + arg_) {}

auto
CMissingArgumentError::Message() const noexcept -> str {
    return m_msg.c_str();
}

CUnknownOptionError::CUnknownOptionError(String const& arg_) : m_msg("Unknown option " + arg_) {}

auto
CUnknownOptionError::Message() const noexcept -> str {
    return m_msg.c_str();
}

auto
CMissingOptionError::Message() const noexcept -> str {
    return "Required option missing";
}

auto
COptsCalledForHelp::Message() const noexcept -> str {
    return "";
}
}  // namespace error

void
CProgramOptions::Parse(usize argc_, str* argv_) {
    m_opts.clear();
    auto const args{tokenizeArgs(argc_, argv_)};
    m_progname = argv_[0];
    for (auto i{1UL}; i < args.size(); ++i) {
        evalArg(args[i], [&]() -> String const& {
            try {
                return args.at(++i);
            } catch ([[maybe_unused]] std::out_of_range const&) {
                throw error::CMissingArgumentError(args[i - 1]);
            }
        });
    }
}

auto
CProgramOptions::GetOpt(str key_) const -> std::optional<String> {
    try {
        return m_opts.at(key_);
    } catch ([[maybe_unused]] std::out_of_range const&) {
        return std::nullopt;
    }
}

auto
CProgramOptions::RequireOpt(str key_) const -> std::optional<String> {
    try {
        return m_opts.at(key_);
    } catch ([[maybe_unused]] std::out_of_range const&) {
        throw error::CMissingOptionError();
    }
}

void
CProgramOptions::printHelp() {
    std::cout << "VirtualFlightRadar-Backend -- " << VFRB_VERSION
              << "\nGitHub: https://github.com/Jarthianur/VirtualFlightRadar-Backend\n\n"
              << "Usage: " << m_progname << " OPTIONS\n\n"
              << "OPTIONS:\n"
                 "  --help | -h            : Show this message and exit.\n"
                 "  --config | -c <file>   : Specify the configuration file. (required!)\n"
                 "  --ground | -g          : Force ground mode.\n"
                 "  --log-file | -l <file> : Specify a log file."
              << std::endl;
    throw error::COptsCalledForHelp();
}

auto
CProgramOptions::tokenizeArgs(usize argc_, str* argv_) -> Vector<String> {
    Vector<String> a;
    a.reserve(argc_);
    for (auto i{0UL}; i < argc_; ++i) {
        String arg(argv_[i]);
        if (!arg.empty()) {
            a.push_back(std::move(arg));
        }
    }
    return a;
}

}  // namespace vfrb
