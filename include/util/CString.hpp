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

    void copy(std::string_view const& sv_)
    {
        usize len = sv_.length();
        if (len > N)
        {
            throw error::OverflowError();
        }
        std::copy_n(sv_.cbegin(), len, m_data.begin());
        if (m_data[len - 1] != '\0')
        {
            if (len < N)
            {
                m_data[len++] = '\0';
            }
            else
            {
                throw error::OverflowError();
            }
        }
        m_view = std::string_view(m_data.data(), len);
    }

    void copy(CString const& other_)
    {
        std::copy(other_.m_data.cbegin(), other_.m_data.cend(), m_data.begin());
        m_view = std::string_view(m_data.data(), other_.m_view.length());
    }

public:
    CString()
    {
        Clear();
    }

    CString(char const* init_)  ///< @param init The initial cstring to copy
    {
        operator=(init_);
    }

    CString(Str const& init_)  ///< @param init The initial string to copy
    {
        operator=(init_);
    }

    CString(CString const& other_)  ///< @param other The other CString to copy
    {
        operator=(other_);
    }

    CString(std::string_view const& other_)  ///< @param other The other CString to copy
    {
        operator=(other_);
    }

    ~CString() noexcept = default;

    CString& operator=(char const* other_)
    {
        copy(std::string_view(other_));
        return *this;
    }

    CString& operator=(Str const& other_)
    {
        copy(std::string_view(other_));
        return *this;
    }

    CString& operator=(CString const& other_)
    {
        copy(other_);
        return *this;
    }

    CString& operator=(std::string_view const& other_)
    {
        copy(other_);
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

    bool operator==(CString const& other_) const
    {
        return m_view == other_.m_view;
    }

    void Clear()
    {
        m_data[0] = '\0';
        m_view    = std::string_view(m_data.data(), 0);
    }

    int Format(usize pos_, char const* fmt_, ...)
    {
        if (pos_ >= N)
        {
            throw error::OverflowError();
        }
        usize   max = N - pos_;
        va_list args;
        va_start(args, fmt_);
        int b = 0;
        if ((b = std::vsnprintf(m_data.data() + pos_, max, fmt_, args)) >= 0)
        {
            m_view = std::string_view(m_data.data(), pos_ + b + 1);
        }
        va_end(args);
        if (b < 0)
        {
            Clear();
            throw error::OverflowError();
        }
        return b;
    }

    inline usize Length() const
    {
        return m_view.length();
    }
};
}  // namespace vfrb::util
