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

#include "ClimateData.h"

ClimateData::ClimateData()
        : pressure(ICAO_STD_A),
          temperature(ICAO_STD_T)
{
}

ClimateData::~ClimateData()
{
}

std::string ClimateData::extractWV()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    wv_valid = false;
    return wv_ + "\r\n";
}

bool ClimateData::isValid()
{
    return wv_valid;
}

double ClimateData::getPress()
{
    return pressure;
}

void ClimateData::insertWV(const std::string& wv)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    wv_ = wv;
    wv_valid = true;
}

void ClimateData::setPress(double p)
{
    pressure = p;
}

void ClimateData::setTemp(double t)
{
    temperature = t;
}

double ClimateData::getTemp()
{
    return temperature;
}
