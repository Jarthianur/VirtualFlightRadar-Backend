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

#pragma once

#include <algorithm>
#include <iostream>

#include "error/IError.hpp"

#include "Types.hpp"

namespace vfrb
{
namespace error
{
class CMissingArgumentError : public IError
{
    String const m_msg;

public:
    explicit CMissingArgumentError(String const& arg_);

    [[nodiscard]] auto
    Message() const noexcept -> str override;
};
}  // namespace error

class CProgramOptions
{
public:
    struct SHelpCalled
    {};

    void
    parse(usize argc_, str* argv_) {
        auto const args{tokenize_args(argc_, argv_)};
        m_progname = argv_[0];
        for (auto i{1UL}; i < args.size(); ++i) {
            eval_arg(args[i], [&](String const& arg_) -> String const& {
                try {
                    return args.at(++i);
                } catch ([[maybe_unused]] std::out_of_range const&) {
                    throw std::runtime_error(arg_ + " requires an argument!");
                }
            });
        }
    }

    inline auto
    config() const -> config const& {
        return m_cfg;
    }

private:
    struct matched
    {};

    template<typename T>
    struct option
    {
        T m_flag;

        template<typename Arg, typename Fn>
        auto
        operator()(Arg&& arg_, Fn&& fn_) const -> decltype(*this)& {
            if (arg_ == m_flag) {
                fn_();
                throw matched{};
            }
            return *this;
        }
    };

    struct combined_option
    {
        template<typename Fn>
        auto
        operator()(std::string const& arg_, Fn&& fn_) const -> decltype(*this)& {
            if (arg_[0] == '-') {
                std::for_each(arg_.cbegin() + 1, arg_.cend(), [&](char c_) {
                    try {
                        fn_(c_);
                    } catch (matched) {
                    }
                });
                throw matched{};
            }
            return *this;
        }
    };

    template<typename Fn>
    void
    eval_arg(std::string const& arg_, Fn&& getval_fn_) {
        try {
            make_option(+"--help")(arg_, [&] { print_help(); });
            make_option(+"--xml")(arg_, [&] { m_cfg.report_fmt = config::report_format::XML; });
            make_option(+"--md")(arg_, [&] { m_cfg.report_fmt = config::report_format::MD; });
            make_option(+"--json")(arg_, [&] { m_cfg.report_fmt = config::report_format::JSON; });
            combined_option{}(arg_, [&](char c_) {
                make_option('c')(c_, [&] { m_cfg.report_cfg.color = true; });
                make_option('o')(c_, [&] { m_cfg.report_cfg.capture_out = true; });
                make_option('s')(c_, [&] { m_cfg.report_cfg.strip = true; });
                make_option('e')(c_, [&] {
                    set_filter_mode(config::filter_mode::EXCLUDE);
                    m_cfg.f_patterns.push_back(to_regex(getval_fn_(arg_)));
                });
                make_option('i')(c_, [&] {
                    set_filter_mode(config::filter_mode::INCLUDE);
                    m_cfg.f_patterns.push_back(to_regex(getval_fn_(arg_)));
                });
            });
        } catch (matched) {
            return;
        }
        m_cfg.report_cfg.outfile = arg_;
    }

    template<typename T>
    static auto
    make_option(T&& t_) -> option<T> {
        return option<T>{std::forward<T>(t_)};
    }

    void
    print_help() {
        std::cout
            << "Unit testing binary built with Test++ (" TPP_VERSION ").\n"
            << "GitHub: https://github.com/Jarthianur/TestPlusPlus\n\n"
            << "Usage: " << m_progname << " [OPTIONS] [filename]\n"
            << "Default is to report to standard-out in an informative text format (using console-reporter).\n\n"
               "OPTIONS:\n"
               "  --help: Print this message and exit.\n"
               "  --xml : Report in JUnit-like XML format.\n"
               "  --md  : Report in markdown format.\n"
               "  --json: Report in json format.\n"
               "  -c    : Use ANSI colors in report, if supported by reporter.\n"
               "  -s    : Strip unnecessary whitespaces from report.\n"
               "  -o    : Report captured output from tests, if supported by reporter.\n\n"
               "  Multiple filters are possible, but includes and excludes are mutually exclusive.\n"
               "  Patterns may contain * as wildcard.\n\n"
               "  -e <pattern> : Exclude testsuites with names matching pattern.\n"
               "  -i <pattern> : Include only testsuites with names matching pattern."
            << std::endl;
        throw SHelpCalled{};
    }

    static auto
    tokenize_args(std::size_t argc_, char const** argv_) -> std::vector<std::string> {
        if (argc_ == 0) {
            throw std::runtime_error("Too few arguments!");
        }
        std::vector<std::string> a;
        a.reserve(argc_);
        for (auto i{0UL}; i < argc_; ++i) {
            std::string arg(argv_[i]);
            if (!arg.empty()) {
                a.push_back(std::move(arg));
            }
        }
        return a;
    }

    struct config m_cfg;
    char const*   m_progname{nullptr};
};
}  // namespace vfrb
