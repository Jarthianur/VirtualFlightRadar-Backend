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

namespace object
{
TimeStamp::TimeStamp() : mValue(0)
{}

TimeStamp::TimeStamp(const std::string& crValue, Format vFormat)
{
    std::int32_t h = 99;
    std::int32_t m = 99;
    std::int32_t s = 99;
    std::int32_t f = 9999;
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
    mValue = static_cast<std::uint32_t>(h * 3600000 + m * 60000 + s * 1000 + f);
}

TimeStamp::~TimeStamp() noexcept
{}

TimeStamp& TimeStamp::operator=(TimeStamp& rOther)
{
    this->mValue = rOther.mValue;
    return *this;
}

bool TimeStamp::operator<(const TimeStamp& crOther) const
{
    return this->mValue < crOther.mValue;
}

bool TimeStamp::operator<=(const TimeStamp& crOther) const
{
    return this->mValue <= crOther.mValue;
}

bool TimeStamp::operator==(const TimeStamp& crOther) const
{
    return this->mValue == crOther.mValue;
}
}
