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
enum class Errc : enum_t
{
    OK,
    ERR
};

template<typename T>
using Result = std::tuple<T, Errc>;

struct x32
{};

namespace error
{
class ConversionError : public vfrb::error::Error
{
public:
    ConversionError()                    = default;
    ~ConversionError() noexcept override = default;

    char const* what() const noexcept override
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
inline s32 checksum(std::string_view const& sv, usize pos)
{
    s32   csum = 0;
    usize i    = 1 + pos;  // $ in nmea str not included
    while (i < sv.length() && sv[i] != '*')
    {
        csum ^= static_cast<s32>(sv[i++]);
    }
    return csum;
}

inline bool matchChecksum(std::string_view const& sv)
{
    auto const cs_begin = sv.rfind('*');
    if (cs_begin == std::string_view::npos || cs_begin + 3 >= sv.length())
    {
        return false;
    }
    s32  csum;
    bool match = false;
    if (auto [p, ec] = std::from_chars(sv.data() + cs_begin + 1, sv.data() + sv.length(), csum, 16);
        ec == std::errc())
    {
        match = csum == checksum(sv, 0);
    }
    return match;
}

template<typename T, ENABLE_IF(IS_TYPE(T, f64))>
Result<T> convert(char const* first, char const* last)
{
    T    result;
    Errc ec = Errc::OK;
    if (!boost::spirit::qi::parse(first, last, boost::spirit::qi::double_, result) || first != last)
    {
        ec = Errc::ERR;
    }
    return {result, ec};
}

template<typename T, ENABLE_IF(IS_TYPE(T, s32))>
Result<T> convert(char const* first, char const* last)
{
    T    result;
    Errc ec = Errc::OK;
    if (auto [p, e] = std::from_chars(first, last, result); e != std::errc())
    {
        ec = Errc::ERR;
    }
    return {result, ec};
}

template<typename T, ENABLE_IF(IS_TYPE(T, u32))>
Result<T> convert(char const* first, char const* last)
{
    T    result;
    Errc ec = Errc::OK;
    if (!boost::spirit::qi::parse(first, last, boost::spirit::qi::uint_, result) || first != last)
    {
        ec = Errc::ERR;
    }
    return {result, ec};
}

template<typename T, ENABLE_IF(IS_TYPE(T, u64))>
Result<T> convert(char const* first, char const* last)
{
    T    result;
    Errc ec = Errc::OK;
    if (!boost::spirit::qi::parse(first, last, boost::spirit::qi::ulong_, result) || first != last)
    {
        ec = Errc::ERR;
    }
    return {result, ec};
}

template<typename T, ENABLE_IF(IS_TYPE(T, x32))>
Result<s32> convert(char const* first, char const* last)
{
    s32  result;
    Errc ec = Errc::OK;
    if (auto [p, e] = std::from_chars(first, last, result, 16); e != std::errc())
    {
        ec = Errc::ERR;
    }
    return {result, ec};
}

template<typename T, ENABLE_IF(IS_TYPE(T, s8))>
Result<T> convert(char const* first, char const* last)
{
    T    result;
    Errc ec = Errc::OK;
    if (!boost::spirit::qi::parse(first, last, boost::spirit::qi::byte_, result) || first != last)
    {
        ec = Errc::ERR;
    }
    return {result, ec};
}

template<typename T, ENABLE_IF(IS_TYPE(T, u8))>
Result<T> convert(char const* first, char const* last)
{
    auto [v, ec] = convert<s8>(first, last);
    return {static_cast<u8>(v), ec};
}

template<typename T, ENABLE_IF(IS_TYPE(T, u16))>
Result<T> convert(char const* first, char const* last)
{
    T    result;
    Errc ec = Errc::OK;
    if (!boost::spirit::qi::parse(first, last, boost::spirit::qi::ushort_, result) || first != last)
    {
        ec = Errc::ERR;
    }
    return {result, ec};
}

template<typename T>
Errc convert(char const* first, char const* last, T& dest)
{
    auto [v, ec] = convert<T>(first, last);
    dest         = v;
    return ec;
}

template<typename T>
auto parse(std::csub_match const& sub)
{
    if (auto [v, ec] = convert<T>(sub.first, sub.second); ec == Errc::OK)
    {
        return v;
    }
    throw error::ConversionError();
}

template<typename T>
auto parse(str const& s)
{
    if (auto [v, ec] = convert<T>(s.c_str(), s.c_str() + s.size()); ec == Errc::OK)
    {
        return v;
    }
    throw error::ConversionError();
}

inline std::string_view to_str_view(std::csub_match const& sub)
{
    return std::string_view(sub.first, static_cast<usize>(sub.second - sub.first));
}

inline bool operator==(std::csub_match const& sub, char const* s)
{
    return to_str_view(sub) == s;
}
}  // namespace vfrb::str_util
