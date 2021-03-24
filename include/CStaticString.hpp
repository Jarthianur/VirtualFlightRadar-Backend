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
#include <cstdio>

#include "error/IError.hpp"

namespace vfrb
{
namespace error
{
class COverflowError : public vfrb::error::IError
{
public:
    [[nodiscard]] auto
    Message() const noexcept -> str override {
        return "";
    }
};
}  // namespace error

template<usize N>
class CStaticString final
{
    Array<char, N> m_data;
    StringView     m_view;

    void
    copy(StringView const& sv_) {
        usize len = sv_.length();
        if (len > N) {
            throw error::COverflowError();
        }
        std::copy_n(sv_.cbegin(), len, m_data.begin());
        m_view = StringView{m_data.data(), len};
    }

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

    void
    Clear() {
        m_view = StringView{m_data.data(), 0};
    }

    [[nodiscard]] inline auto
    Length() const -> usize {
        return m_view.length();
    }
};
}  // namespace vfrb

namespace std
{
template<vfrb::usize N>
struct hash<typename vfrb::CStaticString<N>>
{
    std::size_t
    operator()(typename vfrb::CStaticString<N> const& s) const noexcept {
        return std::hash<vfrb::StringView>()(*s);
    }
};
}  // namespace std
