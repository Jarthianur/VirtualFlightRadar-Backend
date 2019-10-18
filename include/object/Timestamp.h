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
class TimestampParseError : public vfrb::error::Error
{
public:
    TimestampParseError()                    = default;
    ~TimestampParseError() noexcept override = default;

    char const* what() const noexcept override;
};
}  // namespace error

/**
 * @brief A timestamp
 * @tparam DateTimeT The provider of time functions
 */
class Timestamp
{
    s64 m_value = 0;  ///< Time in milliseconds
    u32 m_day   = 0;  ///< Incremental day number

public:
    Timestamp() = default;
    /**
     * @param value  The time string
     * @param format The format
     * @throw std::invalid_argument if the time string is invalid
     */
    Timestamp(std::string_view const& value);
    Timestamp(Timestamp const& other);  ///< @param other The other Timestamp
    ~Timestamp() noexcept = default;

    /**
     * @brief Assign other TimeStamps value.
     * @param other The other Timestamp
     * @return this
     */
    Timestamp& operator=(Timestamp const& other);

    /**
     * @brief Compare this value to be less than, or equals others.
     * @param other The other Timestamp
     * @return true if less, or equals, else false
     */
    bool operator>(Timestamp const& other) const;
};
}  // namespace vfrb::object
