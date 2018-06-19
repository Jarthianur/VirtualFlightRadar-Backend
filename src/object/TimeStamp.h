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

#include <cstdint>
#include <stdexcept>
#include <string>

#include "../Defines.h"

/// @namespace object
namespace object
{
/**
 * @class TimeStamp
 * @brief Represent a timestamp as number of milliseconds.
 */
class TimeStamp
{
public:
    /**
     * @enum Format
     * @brief Format of a given time string.
     */
    enum class Format : std::int32_t
    {
        HHMMSS,
        HH_MM_SS_FFF
    };

    DEFAULT_CTOR_DTOR(TimeStamp)

    /**
     * @fn TimeStamp
     * @brief Constructor
     *
     * Create a TimeStamp from string with specific format.
     *
     * @param crValue The time string
     * @param vFormat The format
     * @throw std::invalid_argument if the time string is invalid
     */
    TimeStamp(const std::string& crValue, Format vFormat);

    /**
     * @fn TimeStamp
     * @brief Constructor
     *
     * Copy constructor
     *
     * @param crOther The other TimeStamp
     */
    TimeStamp(const TimeStamp& crOther);

    /**
     * @fn operator =
     * @brief Assign other TimeStamps value.
     * @param crOther The other TimeStamp
     * @return this
     */
    TimeStamp& operator=(const TimeStamp& crOther);

    /**
     * @fn operator <=
     * @brief Compare this value to be less than, or equals others.
     * @param crOther The other TimeStamp
     * @return true if less, or equals, else false
     */
    bool operator>(const TimeStamp& crOther) const;

private:
    std::uint64_t now() const;

    /// @var mValue
    /// The time in milliseconds
    std::uint64_t mValue;

    bool mYesterday;
};
}  // namespace object
