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

#pragma once

#include <regex>
#include <tuple>

#include <boost/spirit/include/qi.hpp>

#include "error/IError.hpp"

#include "class_utils.hpp"

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
    str Message() const noexcept override {
        return "conversion failed";
    }
};
}  // namespace error

usize constexpr len(str s_) {
    return *s_ ? 1 + len(s_ + 1) : 0;
}

usize len(String const& s_) {
    return s_.length();
}

template<typename... Args>
String MakeStr(Args&&... args_) {
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
Result<T> Convert(str first_, str last_) {
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
Result<T> Convert(str first_, str last_) {
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
Result<T> Convert(str first_, str last_) {
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
Result<T> Convert(str first_, str last_) {
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
Result<u32> Convert(str first_, str last_) {
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
Result<T> Convert(str first_, str last_) {
    T     result;
    EErrc ec = EErrc::OK;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::byte_, result) || first_ != last_) {
        ec = EErrc::ERR;
    }
    return {result, ec};
}

/**
 * Parse a charcter sequence as unsigned 8 bit integer.
 * @param first_ Pointer to first character
 * @param last_  Pointer to one past the last character
 * @return the conversion result
 */
template<typename T, ENABLE_IF(IS_TYPE(T, u8))>
Result<T> Convert(str first_, str last_) {
    auto [v, ec] = Convert<s8>(first_, last_);
    return {static_cast<u8>(v), ec};
}

/**
 * Parse a charcter sequence as unsigned 16 bit integer.
 * @param first_ Pointer to first character
 * @param last_  Pointer to one past the last character
 * @return the conversion result
 */
template<typename T, ENABLE_IF(IS_TYPE(T, u16))>
Result<T> Convert(str first_, str last_) {
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
EErrc Convert(str first_, str last_, T& dest_) {
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
auto Parse(std::csub_match const& sub_) {
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
auto Parse(String const& str_) {
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
inline StringView AsStrView(std::csub_match const& sub_) {
    return StringView(sub_.first, static_cast<usize>(sub_.second - sub_.first));
}

/**
 * Compare a regex sub match and a character sequence for equality.
 * @param sub_ The regex sub match
 * @param s_   The character sequence
 * @return true if equal, else false
 */
inline bool operator==(std::csub_match const& sub_, str cstr_) {
    return AsStrView(sub_) == cstr_;
}

/**
 * Compute checksum of nmea string.
 * @param sv_  The string
 * @param pos_ The index to start from
 * @return the checksum
 */
inline u32 Checksum(StringView const& sv_, usize pos_) {
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
inline bool MatchChecksum(StringView const& sv_) {
    auto const cs_begin = sv_.rfind('*');
    if (cs_begin == StringView::npos || cs_begin + 3 >= sv_.length()) {
        return false;
    }
    bool match = false;
    if (auto [v, ec] = Convert<x32>(sv_.data() + cs_begin + 1, sv_.data() + sv_.length()); ec == EErrc::OK) {
        match = (v == Checksum(sv_, 0));
    }
    return match;
}
}  // namespace vfrb::str_util
