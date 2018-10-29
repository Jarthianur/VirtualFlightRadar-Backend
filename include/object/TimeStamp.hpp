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

namespace object
{
namespace timestamp
{
/**
 * @brief Format of a given time string.
 */
enum class Format : std::int8_t
{
    HHMMSS,
    HH_MM_SS_FFF
};

}  // namespace timestamp

/**
 * @brief A timestamp
 * @tparam DateTimeT The provider of time functions
 */
template<typename DateTimeT>
class TimeStamp
{
public:
    /**
     * @brief Constructor
     */
    TimeStamp() = default;

    /**
     * @brief Constructor
     * @param value  The time string
     * @param format The format
     * @throw std::invalid_argument if the time string is invalid
     */
    TimeStamp(const std::string& value, timestamp::Format format);

    /**
     * @brief Copy-Constructor
     * @param other The other TimeStamp
     */
    TimeStamp(const TimeStamp& other);

    /**
     * @brief Destructor
     */
    ~TimeStamp() noexcept = default;

    /**
     * @brief Assign other TimeStamps value.
     * @param other The other TimeStamp
     * @return this
     */
    TimeStamp& operator=(const TimeStamp& other);

    /**
     * @brief Compare this value to be less than, or equals others.
     * @param other The other TimeStamp
     * @return true if less, or equals, else false
     */
    bool operator>(const TimeStamp& other) const;

private:
    /// Time in milliseconds
    std::int64_t m_value = 0;

    /// Incremental day number
    std::uint32_t m_day = 0;
};

template<typename DateTimeT>
TimeStamp<DateTimeT>::TimeStamp(const std::string& value, timestamp::Format format)
    : m_day(DateTimeT::day())
{
    std::int32_t h = 99, m = 99, s = 99, f = 9999;
    try
    {
        switch (format)
        {
            case timestamp::Format::HHMMSS:
            {
                h = std::stoi(value.substr(0, 2));
                m = std::stoi(value.substr(2, 2));
                s = std::stoi(value.substr(4, 2));
                f = 0;
            }
            break;
            case timestamp::Format::HH_MM_SS_FFF:
            {
                h = std::stoi(value.substr(0, 2));
                m = std::stoi(value.substr(3, 2));
                s = std::stoi(value.substr(6, 2));
                f = std::stoi(value.substr(9, 3));
            }
            break;
        }
    }
    catch (const std::out_of_range&)
    {
        throw std::invalid_argument("");
    }
    if (h > 23 || m > 59 || s > 59 || f > 999)
    {
        throw std::invalid_argument("");
    }
    m_value = static_cast<std::int64_t>(h * 3600000 + m * 60000 + s * 1000 + f);
    if (m_value >= DateTimeT::now())
    {
        --m_day;
    }
}

template<typename DateTimeT>
TimeStamp<DateTimeT>::TimeStamp(const TimeStamp<DateTimeT>& other)
    : m_value(other.m_value), m_day(other.m_day)
{}

template<typename DateTimeT>
TimeStamp<DateTimeT>& TimeStamp<DateTimeT>::operator=(const TimeStamp<DateTimeT>& other)
{
    this->m_value = other.m_value;
    this->m_day   = other.m_day;
    return *this;
}

template<typename DateTimeT>
bool TimeStamp<DateTimeT>::operator>(const TimeStamp<DateTimeT>& other) const
{
    return (this->m_day > other.m_day) ||
           ((this->m_day == other.m_day) && this->m_value > other.m_value);
}

}  // namespace object
