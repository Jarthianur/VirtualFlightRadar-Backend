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

#include <charconv>
#include <limits>
#include <regex>
#include <string_view>
#include <tuple>
#include <type_traits>

#include <boost/spirit/include/qi.hpp>

#include "error/Error.hpp"

#include "class_utils.h"
#include "types.h"
#include "utility.hpp"

namespace vfrb::str_util
{
enum class EErrc : enum_type
{
    OK,
    ERR
};

template<typename T>
using Result = std::tuple<T, EErrc>;

struct x32
{};

namespace error
{
class CConversionError : public vfrb::error::IError
{
public:
    CConversionError()                    = default;
    ~CConversionError() noexcept override = default;

    char const* What() const noexcept override
    {
        return "conversion failed";
    }
};
}  // namespace error

/**
 * @brief Compute checksum of nmea string.
 * @param sentence The sentence to eval
 * @param length   The sentences size
 * @return the checksum
 */
inline s32 Checksum(std::string_view const& sv_, usize pos_)
{
    s32   csum = 0;
    usize i    = 1 + pos_;  // $ in nmea str not included
    while (i < sv_.length() && sv_[i] != '*')
    {
        csum ^= static_cast<s32>(sv_[i++]);
    }
    return csum;
}

inline bool MatchChecksum(std::string_view const& sv_)
{
    auto const cs_begin = sv_.rfind('*');
    if (cs_begin == std::string_view::npos || cs_begin + 3 >= sv_.length())
    {
        return false;
    }
    s32  csum;
    bool match = false;
    if (auto [p, ec] = std::from_chars(sv_.data() + cs_begin + 1, sv_.data() + sv_.length(), csum, 16);
        ec == std::errc())
    {
        match = csum == Checksum(sv_, 0);
    }
    return match;
}

template<typename T, ENABLE_IF(IS_TYPE(T, f64))>
Result<T> Convert(char const* first_, char const* last_)
{
    T     result;
    EErrc ec = EErrc::OK;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::double_, result) || first_ != last_)
    {
        ec = EErrc::ERR;
    }
    return {result, ec};
}

template<typename T, ENABLE_IF(IS_TYPE(T, s32))>
Result<T> Convert(char const* first_, char const* last_)
{
    T     result;
    EErrc ec = EErrc::OK;
    if (auto [p, e] = std::from_chars(first_, last_, result); e != std::errc())
    {
        ec = EErrc::ERR;
    }
    return {result, ec};
}

template<typename T, ENABLE_IF(IS_TYPE(T, u32))>
Result<T> Convert(char const* first_, char const* last_)
{
    T     result;
    EErrc ec = EErrc::OK;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::uint_, result) || first_ != last_)
    {
        ec = EErrc::ERR;
    }
    return {result, ec};
}

template<typename T, ENABLE_IF(IS_TYPE(T, u64))>
Result<T> Convert(char const* first_, char const* last_)
{
    T     result;
    EErrc ec = EErrc::OK;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::ulong_, result) || first_ != last_)
    {
        ec = EErrc::ERR;
    }
    return {result, ec};
}

template<typename T, ENABLE_IF(IS_TYPE(T, x32))>
Result<s32> Convert(char const* first_, char const* last_)
{
    s32   result;
    EErrc ec = EErrc::OK;
    if (auto [p, e] = std::from_chars(first_, last_, result, 16); e != std::errc())
    {
        ec = EErrc::ERR;
    }
    return {result, ec};
}

template<typename T, ENABLE_IF(IS_TYPE(T, s8))>
Result<T> Convert(char const* first_, char const* last_)
{
    T     result;
    EErrc ec = EErrc::OK;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::byte_, result) || first_ != last_)
    {
        ec = EErrc::ERR;
    }
    return {result, ec};
}

template<typename T, ENABLE_IF(IS_TYPE(T, u8))>
Result<T> Convert(char const* first_, char const* last_)
{
    auto [v, ec] = Convert<s8>(first_, last_);
    return {static_cast<u8>(v), ec};
}

template<typename T, ENABLE_IF(IS_TYPE(T, u16))>
Result<T> Convert(char const* first_, char const* last_)
{
    T     result;
    EErrc ec = EErrc::OK;
    if (!boost::spirit::qi::parse(first_, last_, boost::spirit::qi::ushort_, result) || first_ != last_)
    {
        ec = EErrc::ERR;
    }
    return {result, ec};
}

template<typename T>
EErrc Convert(char const* first_, char const* last_, T& dest_)
{
    auto [v, ec] = Convert<T>(first_, last_);
    dest_        = v;
    return ec;
}

template<typename T>
auto Parse(std::csub_match const& sub_)
{
    if (auto [v, ec] = Convert<T>(sub_.first, sub_.second); ec == EErrc::OK)
    {
        return v;
    }
    throw error::CConversionError();
}

template<typename T>
auto Parse(Str const& s_)
{
    if (auto [v, ec] = Convert<T>(s_.c_str(), s_.c_str() + s_.size()); ec == EErrc::OK)
    {
        return v;
    }
    throw error::CConversionError();
}

inline std::string_view ToStrView(std::csub_match const& sub_)
{
    return std::string_view(sub_.first, static_cast<usize>(sub_.second - sub_.first));
}

inline bool operator==(std::csub_match const& sub_, char const* s_)
{
    return ToStrView(sub_) == s_;
}
}  // namespace vfrb::str_util
