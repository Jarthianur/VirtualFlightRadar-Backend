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
#include <string_view>
#include <type_traits>

#include "error/Error.hpp"

#include "types.h"

namespace vfrb::util
{
namespace error
{
class LimitsExceededError : public vfrb::error::Error
{
    str const m_msg;

public:
    template<typename T>
    LimitsExceededError(T val, T min, T max)
        : m_msg(std::to_string(val) + " not in [" + std::to_string(min) + "," + std::to_string(max) + "]")
    {}
    ~LimitsExceededError() noexcept override = default;

    char const* what() const noexcept override
    {
        return m_msg.c_str();
    }
};
}  // namespace error

/**
 * @brief Get enum value as the underlying type.
 * @param value The enum value
 * @return the value as its underlyig type
 */
template<typename T>
constexpr auto raw_type(T value) -> typename std::underlying_type<T>::type
{
    return static_cast<typename std::underlying_type<T>::type>(value);
}

template<typename T>
void checkLimits(T val, T min, T max)
{
    if (val < min || val > max)
    {
        throw error::LimitsExceededError(val, min, max);
    }
}

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
}  // namespace vfrb::util
