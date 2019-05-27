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

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <list>
#include <sstream>
#include <string>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include "defines.h"

namespace util
{
/// @typedef Number
/// A variant number, which can have one of multiple representations
using Number = boost::variant<std::int32_t, std::uint64_t, double>;

/// @typedef OptNumber
/// An optional number, which may be invalid
using OptNumber = boost::optional<Number>;

/**
 * @brief Convert a string to number.
 * @tparam T    The number type
 * @param str The string to convert
 * @return an optional number, which may be invalid
 */
template<typename T>
inline OptNumber stringToNumber(const std::string& str)
{
    std::stringstream ss(str);
    T                 result;
    if (ss >> result)
    {
        return Number(result);
    }
    return boost::none;
}

/**
 * @brief Trim a string on both sides.
 * @param str The string to trim
 * @return the trimmed string
 */
inline std::string& trimString(std::string& str)
{
    std::size_t f = str.find_first_not_of(' ');
    if (f != std::string::npos)
    {
        str = str.substr(f);
    }
    std::size_t l = str.find_last_not_of(' ');
    if (l != std::string::npos)
    {
        str = str.substr(0, l + 1);
    }
    return str;
}

/**
 * @brief Split a string, separated at commata.
 * @param str The string to split
 * @return a list of strings
 */
inline std::list<std::string> splitCommaSeparated(const std::string& str)
{
    std::list<std::string> list;
    std::stringstream      ss;
    ss.str(str);
    std::string item;

    while (std::getline(ss, item, ','))
    {
        list.push_back(trimString(item));
    }
    return list;
}

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

using CStringPack = std::pair<const char*, std::size_t>;

template<std::size_t N, typename std::enable_if<(N > 0)>::type* = nullptr>
class CString
{
public:
    static constexpr const std::size_t last = N - 1;

    DEFAULT_DTOR(CString)

    CString()
    {
        m_value[0] = '\0';
    }

    CString(const char* init)
    {
        operator=(init);
    }

    CString(const std::string& init)
    {
        copy(init.c_str(), init.size());
    }

    CString(const CString& other)
    {
        operator=(other);
    }

    CString& operator=(const char* other)
    {
        copy(other, std::strlen(other));
        return *this;
    }

    CString& operator=(const std::string& other)
    {
        copy(other.c_str(), other.size());
        return *this;
    }

    CString& operator=(const CString& other)
    {
        copy(other.m_value, other.m_length);
        return *this;
    }

    char* operator*()
    {
        return m_value;
    }

    const char* operator*() const
    {
        return m_value;
    }

    void clear()
    {
        m_value[0] = '\0';
        m_length   = 0;
    }

    operator CStringPack() const
    {
        return std::make_pair<const char*, std::size_t>(m_value, m_length);
    }

protected:
    void copy(const char* str, std::size_t n)
    {
        m_length = std::min(last, n);
        std::memcpy(m_value, str, m_length);
        m_value[m_length] = '\0';
    }

    char        m_value[N];
    std::size_t m_length = 0;

public:
    GETTER_V(length)
};

}  // namespace util
