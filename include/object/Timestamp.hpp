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

#include <stdexcept>

#include "util/types.h"

namespace object::time
{
/**
 * @brief Format of a given time string.
 */
enum class Format : enum_t
{
    HHMMSS,
    HH_MM_SS_FFF
};

}  // namespace object::time

namespace object
{
/**
 * @brief A timestamp
 * @tparam DateTimeT The provider of time functions
 */
template<typename DateTimeT>
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
    Timestamp(str const& value, time::Format format);
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

template<typename DateTimeT>
Timestamp<DateTimeT>::Timestamp(str const& value, time::Format format) : m_day(DateTimeT::day())
{
    s32 h = 99, m = 99, s = 99, f = 9999;
    try
    {
        switch (format)
        {
            case time::Format::HHMMSS:
            {
                h = std::stoi(value.substr(0, 2));
                m = std::stoi(value.substr(2, 2));
                s = std::stoi(value.substr(4, 2));
                f = 0;
            }
            break;
            case time::Format::HH_MM_SS_FFF:
            {
                h = std::stoi(value.substr(0, 2));
                m = std::stoi(value.substr(3, 2));
                s = std::stoi(value.substr(6, 2));
                f = std::stoi(value.substr(9, 3));
            }
            break;
        }
    }
    catch (std::out_of_range const&)
    {
        throw std::invalid_argument("");
    }
    if (h > 23 || m > 59 || s > 59 || f > 999)
    {
        throw std::invalid_argument("");
    }
    m_value = static_cast<s64>(h * 3600000 + m * 60000 + s * 1000 + f);
    if (m_value >= DateTimeT::now())
    {
        --m_day;
    }
}

template<typename DateTimeT>
Timestamp<DateTimeT>::Timestamp(Timestamp<DateTimeT> const& other)
    : m_value(other.m_value), m_day(other.m_day)
{}

template<typename DateTimeT>
Timestamp<DateTimeT>& Timestamp<DateTimeT>::operator=(Timestamp<DateTimeT> const& other)
{
    this->m_value = other.m_value;
    this->m_day   = other.m_day;
    return *this;
}

template<typename DateTimeT>
bool Timestamp<DateTimeT>::operator>(Timestamp<DateTimeT> const& other) const
{
    return (this->m_day > other.m_day) || ((this->m_day == other.m_day) && this->m_value > other.m_value);
}
}  // namespace object
