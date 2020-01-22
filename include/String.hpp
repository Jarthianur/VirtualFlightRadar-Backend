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

#include "error/Error.hpp"

#include "types.h"

namespace vfrb
{
namespace error
{
/// Error to indicate an overflow would occur.
class COverflowError : public vfrb::error::IError
{
public:
    COverflowError()                    = default;
    ~COverflowError() noexcept override = default;

    char const* Message() const noexcept override
    {
        return "";
    }
};
}  // namespace error

/**
 * A fixed size, stack located string.
 * @tparam N The size
 */
template<usize N>
class CString
{
    std::array<char, N> m_data;  ///< The underlying array
    StrView             m_view;  ///< A view on the data

    /**
     * Copy into own data.
     * @param sv_ The string to copy
     * @throw vfrb::error::COverflowError
     */
    void copy(StrView const& sv_)
    {
        usize len = sv_.length();
        if (len > N)
        {
            throw error::COverflowError();
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
                throw error::COverflowError();
            }
        }
        m_view = StrView(m_data.data(), len);
    }

    /**
     * Copy into own data.
     * @param other_ The string to copy
     */
    void copy(CString<N> const& other_)
    {
        std::copy(other_.m_data.cbegin(), other_.m_data.cend(), m_data.begin());
        m_view = StrView(m_data.data(), other_.m_view.length());
    }

public:
    CString()
    {
        Clear();
    }

    CString(char const* init_)
    {
        operator=(init_);
    }

    CString(Str const& init_)
    {
        operator=(init_);
    }

    CString(StrView const& init_)
    {
        operator=(init_);
    }

    CString(CString<N> const& other_)
    {
        operator=(other_);
    }

    ~CString() noexcept = default;

    CString& operator=(char const* other_)
    {
        copy(StrView(other_));
        return *this;
    }

    CString& operator=(Str const& other_)
    {
        copy(StrView(other_));
        return *this;
    }

    CString& operator=(CString<N> const& other_)
    {
        copy(other_);
        return *this;
    }

    CString& operator=(StrView const& other_)
    {
        copy(other_);
        return *this;
    }

    StrView const& operator*() const
    {
        return m_view;
    }

    operator StrView() const
    {
        return m_view;
    }

    bool operator==(CString<N> const& other_) const
    {
        return m_view == other_.m_view;
    }

    /// Clear this string, thus making it to look empty.
    void Clear()
    {
        m_data[0] = '\0';
        m_view    = StrView(m_data.data(), 0);
    }

    /**
     * Write a formatted string into this.
     * @param pos_ The index to start writing
     * @param fmt_ The format string
     * @return the amount of bytes written
     * @throw vfrb::error::COverflowError
     */
    int Format(usize pos_, char const* fmt_, ...)
    {
        if (pos_ >= N)
        {
            throw error::COverflowError();
        }
        usize   max = N - pos_;
        va_list args;
        va_start(args, fmt_);
        int b = 0;
        if ((b = std::vsnprintf(m_data.data() + pos_, max, fmt_, args)) >= 0)
        {
            m_view = StrView(m_data.data(), pos_ + b + 1);
        }
        va_end(args);
        if (b < 0)
        {
            Clear();
            throw error::COverflowError();
        }
        return b;
    }

    /// Get the length of seen characters.
    inline usize Length() const
    {
        return m_view.length();
    }
};
}  // namespace vfrb
