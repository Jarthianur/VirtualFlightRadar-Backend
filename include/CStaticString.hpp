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
    [[nodiscard]] auto
    Message() const noexcept -> str override {
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
    void
    copy(StringView const& sv_) {
        usize len = sv_.length();
        if (len > N) {
            throw error::COverflowError();
        }
        std::copy_n(sv_.cbegin(), len, m_data.begin());
        m_view = StringView{m_data.data(), len};
    }

    /**
     * Copy into own data.
     * @param other_ The string to copy
     */
    void
    copy(CStaticString<N> const& other_) {
        std::copy(other_.m_data.cbegin(), other_.m_data.cend(), m_data.begin());
        m_view = StringView{m_data.data(), other_.m_view.length()};
    }

public:
    CStaticString() {
        Clear();
    }

    explicit CStaticString(str init_) {
        copy(StringView{init_});
    }

    explicit CStaticString(String const& init_) {
        copy(StringView{init_});
    }

    explicit CStaticString(StringView const& init_) {
        copy(init_);
    }

    CStaticString(CStaticString<N> const& other_) {
        copy(other_);
    }

    CStaticString(CStaticString<N>&& other_) noexcept {
        copy(other_);
    }

    ~CStaticString() noexcept = default;

    auto
    operator=(str other_) -> CStaticString& {
        copy(StringView{other_});
        return *this;
    }

    auto
    operator=(String const& other_) -> CStaticString& {
        copy(StringView{other_});
        return *this;
    }

    auto
    operator=(CStaticString<N> const& other_) -> CStaticString& {
        if (this != &other_) {
            copy(other_);
        }
        return *this;
    }

    auto
    operator=(StringView const& other_) -> CStaticString& {
        copy(other_);
        return *this;
    }

    auto
    operator=(CStaticString<N>&& other_) noexcept -> CStaticString& {
        if (this != &other_) {
            copy(other_);
        }
        return *this;
    }

    auto
    operator*() const -> StringView const& {
        return m_view;
    }

    explicit operator StringView() const {
        return m_view;
    }

    auto
    operator==(CStaticString<N> const& other_) const -> bool {
        return m_view == other_.m_view;
    }

    /// Clear this string, thus making it to look empty.
    void
    Clear() {
        m_view = StringView{m_data.data(), 0};
    }

    /**
     * Write a formatted string into this.
     * @param pos_ The index to start writing
     * @param fmt_ The format string
     * @return the amount of bytes written
     * @throw vfrb::error::COverflowError
     */
    template<typename... Args>
    auto
    Format(usize pos_, str fmt_, Args... args_) -> s32 {
        if (pos_ >= N) {
            throw error::COverflowError();
        }
        usize max = N - pos_;
        s32   b   = 0;
        if ((b = std::snprintf(m_data.data() + pos_, max, fmt_, std::forward<Args>(args_)...)) >= 0 &&
            static_cast<usize>(b) < max) {
            m_view = StringView{m_data.data(), pos_ + b};  // like append
        } else {
            Clear();
            throw error::COverflowError();
        }
        return b;
    }

    /// Get the length of seen characters.
    [[nodiscard]] inline auto
    Length() const -> usize {
        return m_view.length();
    }
};
}  // namespace vfrb
