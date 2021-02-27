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

#include <charconv>
#include <sstream>
#include <stdexcept>

#include "boost/fusion/adapted/std_tuple.hpp"
#include "boost/spirit/include/qi.hpp"

#include "profiler.hpp"

namespace vfrb::profiling
{
template<typename Iter>
auto
parseTime(Iter& first, Iter last) -> Tuple<u32, u32, u32, u32> {
    namespace qi = boost::spirit::qi;

    static const qi::int_parser<u32, 10, 2, 2>                               _2digit = {};
    static const qi::int_parser<u32, 10, 3, 3>                               _3digit = {};
    static const qi::rule<Iter, Tuple<u32, u32, u32, u32>(), qi::space_type> time_r =
        (_2digit >> _2digit >> _2digit >> qi::attr(0)) |
        (_2digit >> ":" >> _2digit >> ":" >> _2digit >> "." >> _3digit);

    Tuple<u32, u32, u32, u32> time;
    if (qi::phrase_parse(first, last, time_r, qi::space, time) && (first == last)) {
        return time;
    }
    throw std::logic_error("");
}

auto
generateTS(bool format) -> String {
    static int        h = 0, m = 0, s = 0;
    std::stringstream ts;

    if (format) {
        ts << (h < 10 ? "0" : "") << h << (m < 10 ? "0" : "") << m << (s < 10 ? "0" : "") << s;
    } else {
        auto f = ((h + m + s) * 10) % 1000;
        ts << (h < 10 ? "0" : "") << h << ":" << (m < 10 ? "0" : "") << m << ":" << (s < 10 ? "0" : "") << s
           << "."
           << (f < 10  ? "00" :
               f < 100 ? "0" :
                         "")
           << f;
    }
    ++s;
    if (s >= 60) {
        ++m;
        s = 0;
    }
    if (m >= 60) {
        ++h;
        m = 0;
    }
    if (h >= 24) {
        s = m = h = 0;
    }
    return ts.str();
}

auto
parseTimeStoI(String const& value) -> i64 {
    i32 h = 99, m = 99, s = 99, f = 9999;
    if (value.length() > 3 && value[2] == ':') {
        h = std::stoi(value.substr(0, 2));
        m = std::stoi(value.substr(3, 2));
        s = std::stoi(value.substr(6, 2));
        f = std::stoi(value.substr(9, 3));
    } else {
        h = std::stoi(value.substr(0, 2));
        m = std::stoi(value.substr(2, 2));
        s = std::stoi(value.substr(4, 2));
        f = 0;
    }
    if (h > 23 || m > 59 || s > 59 || f > 999) {
        throw std::logic_error("");
    }
    return static_cast<i64>(h * 3600000 + m * 60000 + s * 1000 + f);
}

auto
parseTimeCharconv(std::string const& value) -> i64 {
    i32 h = 99, m = 99, s = 99, f = 9999;
    if (value.length() > 3 && value[2] == ':') {
        if (auto [p, e] = std::from_chars(value.c_str(), value.c_str() + 2, h); e != std::errc()) {
            throw std::logic_error("");
        }
        if (auto [p, e] = std::from_chars(value.c_str() + 3, value.c_str() + 5, m); e != std::errc()) {
            throw std::logic_error("");
        }
        if (auto [p, e] = std::from_chars(value.c_str() + 6, value.c_str() + 8, s); e != std::errc()) {
            throw std::logic_error("");
        }
        if (auto [p, e] = std::from_chars(value.c_str() + 9, value.c_str() + 12, f); e != std::errc()) {
            throw std::logic_error("");
        }
    } else {
        if (auto [p, e] = std::from_chars(value.c_str(), value.c_str() + 2, h); e != std::errc()) {
            throw std::logic_error("");
        }
        if (auto [p, e] = std::from_chars(value.c_str() + 2, value.c_str() + 4, m); e != std::errc()) {
            throw std::logic_error("");
        }
        if (auto [p, e] = std::from_chars(value.c_str() + 4, value.c_str() + 6, s); e != std::errc()) {
            throw std::logic_error("");
        }
        f = 0;
    }
    if (h > 23 || m > 59 || s > 59 || f > 999) {
        throw std::logic_error("");
    }
    return static_cast<i64>(h * 3600000 + m * 60000 + s * 1000 + f);
}

constexpr auto TEST_SIZE = 100000;

static auto const& p_ts1 = ProfilerBuilder()
                               .withDescription("parseTime (HHMMSS)       [Spirit.Qi]")
                               .withGroup("Timestamp")
                               .withTestSize(TEST_SIZE)
                               .withTestFn([](Profiler& self) {
                                   self.profile([&self] {
                                       String s  = generateTS(true);
                                       auto   it = s.begin();
                                       self.measure([&] { parseTime(it, s.end()); });
                                   });
                               })
                               .build();

static auto const& p_ts2 = ProfilerBuilder()
                               .withDescription("parseTime (HH:MM:SS.FFF) [Spirit.Qi]")
                               .withGroup("Timestamp")
                               .withTestSize(TEST_SIZE)
                               .withTestFn([](Profiler& self) {
                                   self.profile([&self] {
                                       String s  = generateTS(false);
                                       auto   it = s.begin();
                                       self.measure([&] { parseTime(it, s.end()); });
                                   });
                               })
                               .build();

static auto const& p_ts3 = ProfilerBuilder()
                               .withDescription("parseTime (HHMMSS)       [charconv ]")
                               .withGroup("Timestamp")
                               .withTestSize(TEST_SIZE)
                               .withTestFn([](Profiler& self) {
                                   self.profile([&self] {
                                       auto s = generateTS(true);
                                       self.measure([&] { parseTimeCharconv(s); });
                                   });
                               })
                               .build();

static auto const& p_ts4 = ProfilerBuilder()
                               .withDescription("parseTime (HH:MM:SS.FFF) [charconv ]")
                               .withGroup("Timestamp")
                               .withTestSize(TEST_SIZE)
                               .withTestFn([](Profiler& self) {
                                   self.profile([&self] {
                                       auto s = generateTS(false);
                                       self.measure([&] { parseTimeCharconv(s); });
                                   });
                               })
                               .build();

static auto const& p_ts5 = ProfilerBuilder()
                               .withDescription("parseTime (HHMMSS)       [stoi     ]")
                               .withGroup("Timestamp")
                               .withTestSize(TEST_SIZE)
                               .withTestFn([](Profiler& self) {
                                   self.profile([&self] {
                                       auto s = generateTS(true);
                                       self.measure([&] { parseTimeStoI(s); });
                                   });
                               })
                               .build();

static auto const& p_ts6 = ProfilerBuilder()
                               .withDescription("parseTime (HH:MM:SS.FFF) [stoi     ]")
                               .withGroup("Timestamp")
                               .withTestSize(TEST_SIZE)
                               .withTestFn([](Profiler& self) {
                                   self.profile([&self] {
                                       auto s = generateTS(false);
                                       self.measure([&] { parseTimeStoI(s); });
                                   });
                               })
                               .build();
}  // namespace vfrb::profiling
