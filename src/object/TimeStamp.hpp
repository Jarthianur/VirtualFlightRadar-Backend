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

/// @namespace object
namespace object
{
namespace timestamp
{
/**
 * @enum Format
 * @brief Format of a given time string.
 */
enum class Format : std::int8_t
{
    HHMMSS,
    HH_MM_SS_FFF
};

}  // namespace timestamp

/**
 * @class TimeStamp
 * @brief Represent a timestamp as number of milliseconds.
 */
template<typename DateTimeT>
class TimeStamp
{
public:
    TimeStamp();

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
    TimeStamp(const std::string& value, timestamp::Format format);

    /**
     * @fn TimeStamp
     * @brief Constructor
     *
     * Copy constructor
     *
     * @param crOther The other TimeStamp
     */
    TimeStamp(const TimeStamp& other);

    ~TimeStamp() noexcept;

    /**
     * @fn operator =
     * @brief Assign other TimeStamps value.
     * @param crOther The other TimeStamp
     * @return this
     */
    TimeStamp& operator=(const TimeStamp& other);

    /**
     * @fn operator <=
     * @brief Compare this value to be less than, or equals others.
     * @param crOther The other TimeStamp
     * @return true if less, or equals, else false
     */
    bool operator>(const TimeStamp& other) const;

private:
    /// @var mValue
    /// The time in milliseconds
    std::int64_t m_value;

    std::uint32_t m_day;
};

template<typename DateTimeT>
TimeStamp<DateTimeT>::TimeStamp(const std::string& value, timestamp::Format format)
    : m_day(DateTimeT::day())
{
    std::int32_t h = 99, m = 99, s = 99, f = 9999;
    try
    {
        switch(format)
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
    catch(const std::out_of_range&)
    {
        throw std::invalid_argument("");
    }
    if(h > 23 || m > 59 || s > 59 || f > 999)
    {
        throw std::invalid_argument("");
    }
    m_value = static_cast<std::int64_t>(h * 3600000 + m * 60000 + s * 1000 + f);
    if(m_value >= DateTimeT::now())
    {
        --m_day;
    }
}

template<typename DateTimeT>
TimeStamp<DateTimeT>::TimeStamp() : m_value(0), m_day(0)
{}

template<typename DateTimeT>
TimeStamp<DateTimeT>::TimeStamp(const TimeStamp<DateTimeT>& other)
    : m_value(other.m_value), m_day(other.m_day)
{}

template<typename DateTimeT>
TimeStamp<DateTimeT>::~TimeStamp() noexcept
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
    return (this->m_day > other.m_day)
           || ((this->m_day == other.m_day) && this->m_value > other.m_value);
}

}  // namespace object
