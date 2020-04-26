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
#include "util/class_utils.hpp"

#include "types.hpp"

namespace vfrb::object
{
namespace error
{
/// Error to indicate timestamp parsing failure.
class CTimestampParseError : public vfrb::error::IError
{
public:
    str Message() const noexcept override;
};
}  // namespace error

/// A self parsing Timestamp
class CTimestamp
{
    s64 m_value = 0;  ///< Time in milliseconds
    u32 m_day   = 0;  ///< Incremental day number

public:
    DEFAULT_COPYABLE(CTimestamp)
    COPY_ON_MOVE(CTimestamp)

    CTimestamp() = default;

    /**
     * @param sv_ The time string
     * @throw vfrb::object::error::CTimestampParseError
     */
    explicit CTimestamp(StringView const& sv_);
    ~CTimestamp() noexcept = default;

    bool operator>(CTimestamp const& other_) const;
};
}  // namespace vfrb::object
