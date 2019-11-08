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

#include <string_view>

#include "error/Error.hpp"

#include "types.h"

namespace vfrb::object
{
namespace error
{
class CTimestampParseError : public vfrb::error::IError
{
public:
    CTimestampParseError()                    = default;
    ~CTimestampParseError() noexcept override = default;

    char const* What() const noexcept override;
};
}  // namespace error

/**
 * @brief A timestamp
 * @tparam DateTimeT The provider of time functions
 */
class CTimestamp
{
    s64 m_value = 0;  ///< Time in milliseconds
    u32 m_day   = 0;  ///< Incremental day number

public:
    CTimestamp() = default;
    /**
     * @param value  The time string
     * @param format The format
     * @throw std::invalid_argument if the time string is invalid
     */
    CTimestamp(std::string_view const& value_);
    CTimestamp(CTimestamp const& other_);  ///< @param other The other Timestamp
    ~CTimestamp() noexcept = default;

    /**
     * @brief Assign other TimeStamps value.
     * @param other The other Timestamp
     * @return this
     */
    CTimestamp& operator=(CTimestamp const& other_);

    /**
     * @brief Compare this value to be less than, or equals others.
     * @param other The other Timestamp
     * @return true if less, or equals, else false
     */
    bool operator>(CTimestamp const& other_) const;
};
}  // namespace vfrb::object
