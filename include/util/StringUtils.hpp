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

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <iterator>
#include <regex>

#include "boost/spirit/include/qi.hpp"
#include "error/IError.hpp"

#include "ClassUtils.hpp"
#include "Types.hpp"

namespace vfrb
{
struct x32
{};
}  // namespace vfrb

namespace vfrb::str_util
{
namespace error
{
class CConversionError : public vfrb::error::IError
{
public:
    [[nodiscard]] auto
    Message() const noexcept -> str override {
        return "conversion failed";
    }
};
}  // namespace error

auto constexpr inline len(str s_) -> usize {
    return *s_ != '\0' ? 1 + len(s_ + 1) : 0;
}

auto inline len(String const& s_) -> usize {
    return s_.length();
}

template<typename... Args>
auto
MakeString(Args&&... args_) -> String {
    String s;
    s.reserve((0 + ... + len(args_)));
    return (s + ... + args_);
}

template<typename T, ENABLE_IF(IS_TYPE(T, f64))>
auto
Convert(str first_, str last_) -> Result<T> {
    T result;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::double_, result) || first_ != last_) {
        return Err(result);
    }
    return Ok(result);
}

template<typename T, ENABLE_IF(IS_TYPE(T, i32))>
auto
Convert(str first_, str last_) -> Result<T> {
    T result;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::int_, result) || first_ != last_) {
        return Err(result);
    }
    return Ok(result);
}

template<typename T, ENABLE_IF(IS_TYPE(T, u32))>
auto
Convert(str first_, str last_) -> Result<T> {
    T result;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::uint_, result) || first_ != last_) {
        return Err(result);
    }
    return Ok(result);
}

template<typename T, ENABLE_IF(IS_TYPE(T, u64))>
auto
Convert(str first_, str last_) -> Result<T> {
    T result;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::ulong_, result) || first_ != last_) {
        return Err(result);
    }
    return Ok(result);
}

template<typename T, ENABLE_IF(IS_TYPE(T, x32))>
auto
Convert(str first_, str last_) -> Result<u32> {
    static thread_local const boost::spirit::qi::int_parser<u32, 16> hex    = {};
    u32                                                              result = 0;
    if (!boost::spirit::qi::parse(first_, last_, hex, result) || first_ != last_) {
        return Err(result);
    }
    return Ok(result);
}

template<typename T, ENABLE_IF(IS_TYPE(T, i8))>
auto
Convert(str first_, str last_) -> Result<T> {
    i16 result = 0;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::short_, result) || first_ != last_ ||
        result > limits::i8::max() || result < limits::i8::min()) {
        return Err(static_cast<i8>(result));
    }
    return Ok(static_cast<i8>(result));
}

template<typename T, ENABLE_IF(IS_TYPE(T, u8))>
auto
Convert(str first_, str last_) -> Result<T> {
    u16 result = 0;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::ushort_, result) || first_ != last_ ||
        result > limits::u8::max()) {
        return Err(static_cast<u8>(result));
    }
    return Ok(static_cast<u8>(result));
}

template<typename T, ENABLE_IF(IS_TYPE(T, u16))>
auto
Convert(str first_, str last_) -> Result<T> {
    T result;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::ushort_, result) || first_ != last_) {
        return Err(result);
    }
    return Ok(result);
}

template<typename T>
auto
ConvertInto(str first_, str last_, T& dest_) -> EErrc {
    auto [v, ec] = Convert<T>(first_, last_);
    dest_        = v;
    return ec;
}

template<typename T>
auto
Parse(std::csub_match const& sub_) {
    if (auto [v, ec] = Convert<T>(sub_.first, sub_.second); ec == EErrc::OK) {
        return v;
    }
    throw error::CConversionError();
}

template<typename T>
auto
Parse(String const& str_) {
    if (auto [v, ec] = Convert<T>(str_.c_str(), str_.c_str() + str_.size()); ec == EErrc::OK) {
        return v;
    }
    throw error::CConversionError();
}

inline auto
AsStringView(std::csub_match const& sub_) -> StringView {
    return StringView(sub_.first, static_cast<usize>(sub_.second - sub_.first));
}

inline auto
operator==(std::csub_match const& sub_, str cstr_) -> bool {
    return AsStringView(sub_) == cstr_;
}

inline auto
Checksum(String::const_iterator it_, String::const_iterator end_) -> u32 {
    u32 csum = 0;
    if (it_ != end_) {
        ++it_;  // $ in nmea str not included
    }
    while (it_ != end_ && *it_ != '*') {
        csum ^= static_cast<u32>(*it_);
        ++it_;
    }
    return csum;
}

inline auto
MatchChecksum(String const& str_) -> bool {
    auto const cs_begin = str_.rfind('*');
    if (cs_begin == String::npos || cs_begin + 3 > str_.length()) {
        return false;
    }
    bool match = false;
    if (auto [v, ec] = Convert<x32>(str_.data() + cs_begin + 1, str_.data() + cs_begin + 3);
        ec == EErrc::OK) {
        match = (v == Checksum(str_.begin(), str_.end()));
    }
    return match;
}

class StringInserter
{
    std::back_insert_iterator<String> m_iter;
    String&                           m_string;

public:
    explicit StringInserter(String& str_) : m_iter(std::back_inserter(str_)), m_string(str_) {}

    void
    Copy(String const& str_) {
        std::copy(str_.cbegin(), str_.cend(), m_iter);
    }

    template<typename... Args>
    auto
    Format(str fmt_, Args&&... args_) noexcept -> String::const_iterator {
        auto oldLen = m_string.length();
        fmt::format_to(m_iter, fmt_, std::forward<Args>(args_)...);
        return m_string.cbegin() + oldLen;
    }

    auto
    End() -> String::const_iterator {
        return m_string.cend();
    }
};
}  // namespace vfrb::str_util
