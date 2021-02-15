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

#include <limits>
#include <regex>
#include <tuple>

#include <boost/spirit/include/qi.hpp>

#include "error/IError.hpp"

#include "ClassUtils.hpp"

namespace vfrb
{
/// Helper type to use for hex conversion.
struct x32
{};
}  // namespace vfrb

namespace vfrb::str_util
{
/// Error code
enum class EErrc : enum_type
{
    OK,
    ERR
};

/// Result type
template<typename T>
using Result = std::tuple<T, EErrc>;

namespace error
{
/// Error to indicate failed conversion.
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
MakeStr(Args&&... args_) -> String {
    String s;
    s.reserve((0 + ... + len(args_)));
    return (s + ... + args_);
}

/**
 * Parse a charcter sequence as 64 bit floating point.
 * @param first_ Pointer to first character
 * @param last_  Pointer to one past the last character
 * @return the conversion result
 */
template<typename T, ENABLE_IF(IS_TYPE(T, f64))>
auto
Convert(str first_, str last_) -> Result<T> {
    T     result;
    EErrc ec = EErrc::OK;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::double_, result) || first_ != last_) {
        ec = EErrc::ERR;
    }
    return {result, ec};
}

/**
 * Parse a charcter sequence as signed 32 bit integer.
 * @param first_ Pointer to first character
 * @param last_  Pointer to one past the last character
 * @return the conversion result
 */
template<typename T, ENABLE_IF(IS_TYPE(T, s32))>
auto
Convert(str first_, str last_) -> Result<T> {
    T     result;
    EErrc ec = EErrc::OK;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::int_, result) || first_ != last_) {
        ec = EErrc::ERR;
    }
    return {result, ec};
}

/**
 * Parse a charcter sequence as unsigned 32 bit integer.
 * @param first_ Pointer to first character
 * @param last_  Pointer to one past the last character
 * @return the conversion result
 */
template<typename T, ENABLE_IF(IS_TYPE(T, u32))>
auto
Convert(str first_, str last_) -> Result<T> {
    T     result;
    EErrc ec = EErrc::OK;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::uint_, result) || first_ != last_) {
        ec = EErrc::ERR;
    }
    return {result, ec};
}

/**
 * Parse a charcter sequence as unsigned 64 bit integer.
 * @param first_ Pointer to first character
 * @param last_  Pointer to one past the last character
 * @return the conversion result
 */
template<typename T, ENABLE_IF(IS_TYPE(T, u64))>
auto
Convert(str first_, str last_) -> Result<T> {
    T     result;
    EErrc ec = EErrc::OK;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::ulong_, result) || first_ != last_) {
        ec = EErrc::ERR;
    }
    return {result, ec};
}

/**
 * Parse a charcter sequence as signed 32 bit integer from hex.
 * @param first_ Pointer to first character
 * @param last_  Pointer to one past the last character
 * @return the conversion result
 */
template<typename T, ENABLE_IF(IS_TYPE(T, x32))>
auto
Convert(str first_, str last_) -> Result<u32> {
    static thread_local const boost::spirit::qi::int_parser<u32, 16> hex = {};

    u32   result = 0;
    EErrc ec     = EErrc::OK;
    if (!boost::spirit::qi::parse(first_, last_, hex, result) || first_ != last_) {
        ec = EErrc::ERR;
    }
    return {result, ec};
}

/**
 * Parse a charcter sequence as signed 8 bit integer.
 * @param first_ Pointer to first character
 * @param last_  Pointer to one past the last character
 * @return the conversion result
 */
template<typename T, ENABLE_IF(IS_TYPE(T, s8))>
auto
Convert(str first_, str last_) -> Result<T> {
    s16   result = 0;
    EErrc ec     = EErrc::OK;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::short_, result) || first_ != last_ ||
        result > limits::s8::max() || result < limits::s8::min()) {
        ec = EErrc::ERR;
    }
    return {static_cast<s8>(result), ec};
}

/**
 * Parse a charcter sequence as unsigned 8 bit integer.
 * @param first_ Pointer to first character
 * @param last_  Pointer to one past the last character
 * @return the conversion result
 */
template<typename T, ENABLE_IF(IS_TYPE(T, u8))>
auto
Convert(str first_, str last_) -> Result<T> {
    u16   result = 0;
    EErrc ec     = EErrc::OK;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::ushort_, result) || first_ != last_ ||
        result > limits::u8::max()) {
        ec = EErrc::ERR;
    }
    return {static_cast<u8>(result), ec};
}

/**
 * Parse a charcter sequence as unsigned 16 bit integer.
 * @param first_ Pointer to first character
 * @param last_  Pointer to one past the last character
 * @return the conversion result
 */
template<typename T, ENABLE_IF(IS_TYPE(T, u16))>
auto
Convert(str first_, str last_) -> Result<T> {
    T     result;
    EErrc ec = EErrc::OK;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::ushort_, result) || first_ != last_) {
        ec = EErrc::ERR;
    }
    return {result, ec};
}

/**
 * Parse a charcter sequence into destination.
 * @tparam The type to parse
 * @param first_ Pointer to first character
 * @param last_  Pointer to one past the last character
 * @param dest_  The destination variable
 * @return the resulting error code
 */
template<typename T>
auto
Convert(str first_, str last_, T& dest_) -> EErrc {
    auto [v, ec] = Convert<T>(first_, last_);
    dest_        = v;
    return ec;
}

/**
 * Parse and convert from a regex sub match.
 * @tparam The resulting type
 * @param sub_ The sub match
 * @return the conversion result
 * @throw vfrb::str_util::error::CConversionError
 */
template<typename T>
auto
Parse(std::csub_match const& sub_) {
    if (auto [v, ec] = Convert<T>(sub_.first, sub_.second); ec == EErrc::OK) {
        return v;
    }
    throw error::CConversionError();
}

/**
 * Parse and convert from a string.
 * @tparam The resulting type
 * @param str_ The string
 * @return the conversion result
 * @throw vfrb::str_util::error::CConversionError
 */
template<typename T>
auto
Parse(String const& str_) {
    if (auto [v, ec] = Convert<T>(str_.c_str(), str_.c_str() + str_.size()); ec == EErrc::OK) {
        return v;
    }
    throw error::CConversionError();
}

/**
 * Get a string view on a regex sub match.
 * @param sub_ The sub match
 * @return the string view
 */
inline auto
AsStringView(std::csub_match const& sub_) -> StringView {
    return StringView(sub_.first, static_cast<usize>(sub_.second - sub_.first));
}

/**
 * Compare a regex sub match and a character sequence for equality.
 * @param sub_ The regex sub match
 * @param s_   The character sequence
 * @return true if equal, else false
 */
inline auto
operator==(std::csub_match const& sub_, str cstr_) -> bool {
    return AsStringView(sub_) == cstr_;
}

/**
 * Compute checksum of nmea string.
 * @param sv_  The string
 * @param pos_ The index to start from
 * @return the checksum
 */
inline auto
Checksum(StringView const& sv_, usize pos_) -> u32 {
    u32   csum = 0;
    usize i    = 1 + pos_;  // $ in nmea str not included
    while (i < sv_.length() && sv_[i] != '*') {
        csum ^= static_cast<u32>(sv_[i++]);
    }
    return csum;
}

/**
 * Find the checksum in a nmea string and compare it against the computed one.
 * @param sv_ The string
 * @return true if equal, else false
 */
inline auto
MatchChecksum(StringView const& sv_) -> bool {
    auto const cs_begin = sv_.rfind('*');
    if (cs_begin == StringView::npos || cs_begin + 3 > sv_.length()) {
        return false;
    }
    bool match = false;
    if (auto [v, ec] = Convert<x32>(sv_.data() + cs_begin + 1, sv_.data() + cs_begin + 3); ec == EErrc::OK) {
        match = (v == Checksum(sv_, 0));
    }
    return match;
}
}  // namespace vfrb::str_util
