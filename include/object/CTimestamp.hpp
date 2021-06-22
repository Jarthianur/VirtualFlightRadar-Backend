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

#include "error/IError.hpp"
#include "util/ClassUtils.hpp"

#include "Types.hpp"

namespace vfrb::object
{
namespace error
{
class CTimestampParseError : public vfrb::error::IError
{
public:
    [[nodiscard]] auto
    Message() const noexcept -> str override;
};
}  // namespace error

class CTimestamp
{
    i64 m_value;
    i64 m_day;

    static auto
    parseTime(StringView::const_iterator& first_, StringView::const_iterator const& last_)
        -> Tuple<u32, u32, u32, u32>;

public:
    DEFAULT_COPYABLE(CTimestamp)
    COPY_ON_MOVE(CTimestamp)

    CTimestamp();
    explicit CTimestamp(StringView const& sv_);
    ~CTimestamp() noexcept = default;

    auto
    operator>(CTimestamp const& other_) const -> bool;

    auto
    operator==(CTimestamp const& other_) const -> bool;

    auto
    Diff(CTimestamp const& other_) const -> i64;

    auto
    ToString() const -> String {
        return std::to_string(m_value) + "@" + std::to_string(m_day);
    }
};
}  // namespace vfrb::object
