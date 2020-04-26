/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <algorithm>
#include <array>
#include <cstdio>
#include <utility>

#include "error/IError.hpp"

namespace vfrb
{
namespace error
{
/// Error to indicate an overflow would occur.
class COverflowError : public vfrb::error::IError
{
public:
    str Message() const noexcept override {
        return "";
    }
};
}  // namespace error

/**
 * A fixed size, stack located string.
 * @tparam N The size
 */
template<usize N>
class CStaticString final
{
    std::array<char, N> m_data;  ///< The underlying array
    StringView          m_view;  ///< A view on the data

    /**
     * Copy into own data.
     * @param sv_ The string to copy
     * @throw vfrb::error::COverflowError
     */
    void copy(StringView const& sv_) {
        usize len = sv_.length();
        if (len > N) {
            throw error::COverflowError();
        }
        std::copy_n(sv_.cbegin(), len, m_data.begin());
        if (m_data[len - 1] != '\0') {
            if (len < N) {
                m_data[len++] = '\0';
            } else {
                throw error::COverflowError();
            }
        }
        m_view = StringView(m_data.data(), len);
    }

    /**
     * Copy into own data.
     * @param other_ The string to copy
     */
    void copy(CStaticString<N> const& other_) {
        std::copy(other_.m_data.cbegin(), other_.m_data.cend(), m_data.begin());
        m_view = StringView(m_data.data(), other_.m_view.length());
    }

public:
    CStaticString() {
        Clear();
    }

    CStaticString(str init_) {
        operator=(init_);
    }

    CStaticString(String const& init_) {
        operator=(init_);
    }

    CStaticString(StringView const& init_) {
        operator=(init_);
    }

    CStaticString(CStaticString<N> const& other_) {
        operator=(other_);
    }

    CStaticString(CStaticString<N>&& other_) noexcept {
        operator=(other_);
    }

    ~CStaticString() noexcept = default;

    CStaticString& operator=(str other_) {
        copy(StringView(other_));
        return *this;
    }

    CStaticString& operator=(String const& other_) {
        copy(StringView(other_));
        return *this;
    }

    CStaticString& operator=(CStaticString<N> const& other_) {
        copy(other_);
        return *this;
    }

    CStaticString& operator=(StringView const& other_) {
        copy(other_);
        return *this;
    }

    CStaticString& operator=(CStaticString<N>&& other_) noexcept {
        copy(other_);
        return *this;
    }

    StringView const& operator*() const {
        return m_view;
    }

    operator StringView() const {
        return m_view;
    }

    bool operator==(CStaticString<N> const& other_) const {
        return m_view == other_.m_view;
    }

    /// Clear this string, thus making it to look empty.
    void Clear() {
        m_data[0] = '\0';
        m_view    = StringView(m_data.data(), 0);
    }

    /**
     * Write a formatted string into this.
     * @param pos_ The index to start writing
     * @param fmt_ The format string
     * @return the amount of bytes written
     * @throw vfrb::error::COverflowError
     */
    template<typename... Args>
    int Format(usize pos_, str fmt_, Args... args_) {
        if (pos_ >= N) {
            throw error::COverflowError();
        }
        usize max = N - pos_;
        int   b   = 0;
        if ((b = std::snprintf(m_data.data() + pos_, max, fmt_, std::forward<Args>(args_)...)) >= 0) {
            m_view = StringView(m_data.data(), pos_ + b + 1);
        }
        if (b < 0) {
            Clear();
            throw error::COverflowError();
        }
        return b;
    }

    /// Get the length of seen characters.
    inline usize Length() const {
        return m_view.length();
    }
};
}  // namespace vfrb
