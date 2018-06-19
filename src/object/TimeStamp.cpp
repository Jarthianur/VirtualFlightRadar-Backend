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

#include <boost/date_time/posix_time/posix_time.hpp>
#include <stdexcept>

namespace object
{
TimeStamp::TimeStamp() : mValue(0), mYesterday(false)
{}

TimeStamp::TimeStamp(const std::string& crValue, Format vFormat)
{
    std::int32_t h = 99, m = 99, s = 99, f = 9999;
    try
    {
        switch(vFormat)
        {
            case Format::HHMMSS:
            {
                h = std::stoi(crValue.substr(0, 2));
                m = std::stoi(crValue.substr(2, 2));
                s = std::stoi(crValue.substr(4, 2));
                f = 0;
            }
            break;
            case Format::HH_MM_SS_FFF:
            {
                h = std::stoi(crValue.substr(0, 2));
                m = std::stoi(crValue.substr(3, 2));
                s = std::stoi(crValue.substr(6, 2));
                f = std::stoi(crValue.substr(9, 3));
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
    mValue     = static_cast<std::uint64_t>(h * 3600000 + m * 60000 + s * 1000 + f);
    mYesterday = mValue >= now();
}

TimeStamp::TimeStamp(const TimeStamp& crOther)
    : mValue(crOther.mValue), mYesterday(crOther.mYesterday)
{}

TimeStamp::~TimeStamp() noexcept
{}

TimeStamp& TimeStamp::operator=(const TimeStamp& crOther)
{
    this->mValue = crOther.mValue;
    return *this;
}

bool TimeStamp::operator>(const TimeStamp& crOther) const
{
    return (crOther.mYesterday && !this->mYesterday)
           || ((!this->mYesterday || crOther.mYesterday) && this->mValue > crOther.mValue);
}

std::uint64_t TimeStamp::now() const
{
    return static_cast<std::uint64_t>(
        boost::posix_time::time_duration(
            boost::posix_time::microsec_clock::universal_time().time_of_day())
            .total_milliseconds());
}

}  // namespace object
