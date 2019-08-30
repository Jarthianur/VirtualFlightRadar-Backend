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
#include <array>
#include <cstdarg>
#include <cstdio>
#include <string_view>

#include "error/Error.hpp"

#include "types.h"

namespace vfrb::util
{
namespace error
{
class OverflowError : public vfrb::error::Error
{
public:
    OverflowError()                    = default;
    ~OverflowError() noexcept override = default;
    char const* what() const noexcept override
    {
        return "";
    }
};
}  // namespace error

template<usize N>
class CString
{
    std::array<char, N> m_data;
    std::string_view    m_view;

    void copy(std::string_view const& sv)
    {
        if (sv.length() > N)
        {
            throw error::OverflowError();
        }
        std::copy_n(sv.cbegin(), sv.length(), m_data.begin());
        m_view = std::string_view(m_data.data(), sv.length());
    }

    void copy(CString const& other)
    {
        std::copy(other.m_data.cbegin(), other.m_data.cend(), m_data.begin());
        m_view = std::string_view(m_data.data(), other.m_view.length());
    }

public:
    CString()
    {
        clear();
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

    CString(std::string_view const& other)  ///< @param other The other CString to copy
    {
        operator=(other);
    }

    ~CString() noexcept = default;

    CString& operator=(char const* other)
    {
        copy(std::string_view(other));
        return *this;
    }

    CString& operator=(str const& other)
    {
        copy(std::string_view(other));
        return *this;
    }

    CString& operator=(CString const& other)
    {
        copy(other);
        return *this;
    }

    CString& operator=(std::string_view const& other)
    {
        copy(other);
        return *this;
    }

    std::string_view const& operator*() const
    {
        return m_view;
    }

    operator std::string_view() const
    {
        return m_view;
    }

    bool operator==(CString const& other) const
    {
        return m_view == other.m_view;
    }

    void clear()
    {
        m_data[0] = '\0';
        m_view    = std::string_view(m_data.data(), 0);
    }

    int format(usize pos, char const* fmt, ...)
    {
        if (pos >= N)
        {
            throw error::OverflowError();
        }
        usize   max = N - pos;
        va_list args;
        va_start(args, fmt);
        int b = 0;
        if ((b = std::vsnprintf(m_data.data() + pos, max, fmt, args)) >= 0)
        {
            m_view = std::string_view(m_data.data(), pos + b + 1);
        }
        va_end(args);
        if (b < 0)
        {
            clear();
            throw error::OverflowError();
        }
        return b;
    }

    inline usize length() const
    {
        return m_view.length();
    }
};
}  // namespace vfrb::util
