/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

#include "WeatherFeed.h"

#include <stddef.h>
#include <stdexcept>

WeatherFeed::WeatherFeed()
{
    nmea_str = "\r\n";
}

WeatherFeed::~WeatherFeed()
{
}

std::string WeatherFeed::getNMEA()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    wind_valid = false;
    return nmea_str;
}

void WeatherFeed::writeNMEA(const std::string& str)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    if (str.substr(1, 5).compare("WIMWV") == 0)
    {
        nmea_str = str;
        wind_valid = true;
    }
    else if (str.substr(1, 5).compare("WIMDA") == 0)
    {
        size_t b, s;
        try
        {
            b = str.find('B') - 1;
            s = str.substr(0, b).find_last_of(',') + 1;
            pressure = std::stod(str.substr(s, b - s)) * 1000.0;
        }
        catch (std::logic_error& e)
        {
            pressure = A_VALUE_NA;
        }
        try
        {
            b = str.find('C') - 1;
            s = str.substr(0, b).find_last_of(',') + 1;
            temperature = std::stod(str.substr(s, b - s));
        }
        catch (std::logic_error& e)
        {
            temperature = A_VALUE_NA;
        }
    }
}

bool WeatherFeed::isValid()
{
    return wind_valid;
}

double WeatherFeed::getPress()
{
    return pressure;
}

double WeatherFeed::getTemp()
{
    return temperature;
}
