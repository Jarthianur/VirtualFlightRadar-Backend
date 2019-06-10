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
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <type_traits>

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

using CStringPack = std::pair<const char*, std::size_t>;

template<std::size_t N, typename std::enable_if<(N > 0)>::type* = nullptr>
class CString final
{
    //< begin constants >//
    static constexpr auto last = N - 1;
    //< end constants >//

    //< begin members >//
    char        m_value[N];
    std::size_t m_length = 0;
    //< end members >//

    //< begin methods >//
    void copy(const char* str, std::size_t n)
    {
        m_length = std::min(last, n);
        std::memcpy(m_value, str, m_length);
        m_value[m_length] = '\0';
    }
    //< end methods >//

public:
    CString()
    {
        m_value[0] = '\0';
    }

    CString(const char* init)  ///< @param init The initial cstring to copy
    {
        operator=(init);
    }

    CString(const std::string& init)  ///< @param init The initial string to copy
    {
        copy(init.c_str(), init.size());
    }

    CString(const CString& other)  ///< @param other The other CString to copy
    {
        operator=(other);
    }

    ~CString() noexcept = default;

    //< begin operators >//
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

    const char* operator*() const
    {
        return m_value;
    }

    operator CStringPack() const
    {
        return std::make_pair<const char*, std::size_t>(m_value, m_length);
    }

    bool operator==(const CString& other) const
    {
        return std::strncmp(m_value, other.m_value, N) == 0;
    }
    //< end operators >//

    //< begin interfaces >//
    void clear()
    {
        m_value[0] = '\0';
        m_length   = 0;
    }

    int snprintf(std::size_t pos, std::size_t n, const char* fmt, ...)
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

    inline auto getLength() const -> decltype(m_length)
    {
        return m_length;
    }
    //< end interfaces >//
};

}  // namespace util
