/*
Copyright_License {

  Copyright (C) 2016 Virtual Flight Radar - Backend
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

WeatherFeed::WeatherFeed()
{
    nmea_str = "\r\n";
}

WeatherFeed::~WeatherFeed()
{
}

void WeatherFeed::getNMEA(std::string& dest_str)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    dest_str = nmea_str;
    wind_valid = false;
    return;
}

void WeatherFeed::writeNMEA(const std::string& str)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    if (str.substr(0,6).compare("$WIMWV") == 0) {
        nmea_str = str;
        wind_valid = true;
    } else if (str.substr(0,6).compare("$WIMDA") == 0) {
        try {//todo critical read errors!
            pressure = std::stod(str.substr(17,6)) * 1000.0;
        } catch (std::invalid_argument& e) {
            pressure = VALUE_NA;
        }
        try {
            temperature = std::stod(str.substr(26,4));
        } catch (std::invalid_argument& e) {
            temperature = VALUE_NA;
        }
    }
    return;
}

bool WeatherFeed::isValid()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return wind_valid;
}

double WeatherFeed::getQNH()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return pressure;
}

double WeatherFeed::getTemp()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return temperature;
}
