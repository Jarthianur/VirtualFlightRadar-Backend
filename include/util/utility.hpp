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
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "types.h"

namespace util
{
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

using CStringPack = std::pair<char const*, usize const>;

template<usize N, typename std::enable_if<(N > 0)>::type* = nullptr>
class CString final
{
    inline static constexpr auto last = N - 1;

    char  m_value[N];
    usize m_length = 0;

    void copy(char const* str, usize n)
    {
        if (n > last)
        {
            throw std::out_of_range("");
        }
        m_length = n;
        std::memcpy(m_value, str, m_length);
        m_value[m_length] = '\0';
    }

    void copy(CString const& other)
    {
        m_length = other.m_length;
        std::memcpy(m_value, other.m_value, m_length);
        m_value[m_length] = '\0';
    }

public:
    CString()
    {
        m_value[0] = '\0';
    }

    CString(char const* init)  ///< @param init The initial cstring to copy
    {
        operator=(init);
    }

    CString(str const& init)  ///< @param init The initial string to copy
    {
        operator=(init);
    }

    CString(CString const& other)  ///< @param other The other CString to copy
    {
        operator=(other);
    }

    ~CString() noexcept = default;

    CString& operator=(char const* other)
    {
        copy(other, std::strlen(other));
        return *this;
    }

    CString& operator=(str const& other)
    {
        copy(other.c_str(), other.size());
        return *this;
    }

    CString& operator=(CString const& other)
    {
        copy(other);
        return *this;
    }

    char const* operator*() const
    {
        return m_value;
    }

    operator CStringPack() const
    {
        return std::pair<char const*, usize const>(m_value, m_length);
    }

    bool operator==(CString const& other) const
    {
        return std::strncmp(m_value, other.m_value, N) == 0;
    }

    void clear()
    {
        m_value[0] = '\0';
        m_length   = 0;
    }

    int snprintf(usize pos, usize n, char const* fmt, ...)
    {
        if (pos + n >= N)
        {
            throw std::out_of_range("");
        }
        va_list args;
        va_start(args, fmt);
        int b = 0;
        if ((b = std::vsnprintf(m_value + pos, std::min(n, N), fmt, args)) >= 0)
        {
            m_length = b;
        }
        va_end(args);
        if (b < 0)
        {
            clear();
            throw std::out_of_range("");
        }
        return b;
    }

    inline auto length() const -> decltype(m_length)
    {
        return m_length;
    }
};
}  // namespace util
