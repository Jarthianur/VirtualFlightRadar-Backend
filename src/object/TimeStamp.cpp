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

#include "TimeStamp.h"

#include <stdexcept>

namespace object
{
TimeStamp::TimeStamp() : m_value(0), m_day(0)
{}

TimeStamp::TimeStamp(const std::string& value, Format format)
{
    std::int32_t h = 99, m = 99, s = 99, f = 9999;
    try
    {
        switch(format)
        {
            case Format::HHMMSS:
            {
                h = std::stoi(value.substr(0, 2));
                m = std::stoi(value.substr(2, 2));
                s = std::stoi(value.substr(4, 2));
                f = 0;
            }
            break;
            case Format::HH_MM_SS_FFF:
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
    m_value = static_cast<std::uint64_t>(h * 3600000 + m * 60000 + s * 1000 + f);
    m_day   = boost::posix_time::microsec_clock::universal_time().date().modjulian_day();
    if(m_value >= now())
    {
        --m_day;
    }
}

TimeStamp::TimeStamp(const TimeStamp& other) : m_value(other.m_value), m_day(other.m_day)
{}

TimeStamp::~TimeStamp() noexcept
{}

TimeStamp& TimeStamp::operator=(const TimeStamp& other)
{
    this->m_value = other.m_value;
    this->m_day   = other.m_day;
    return *this;
}

bool TimeStamp::operator>(const TimeStamp& other) const
{
    return (this->m_day > other.m_day)
           || ((this->m_day == other.m_day) && this->m_value > other.m_value);
}

std::uint64_t TimeStamp::now() const
{
    return static_cast<std::uint64_t>(
        boost::posix_time::time_duration(
            boost::posix_time::microsec_clock::universal_time().time_of_day())
            .total_milliseconds());
}

}  // namespace object
