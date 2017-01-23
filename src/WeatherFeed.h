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

#ifndef WINDFEED_H_
#define WINDFEED_H_

#include <mutex>
#include <string>

#define ICAO_STD_A 1013.25
#define ICAO_STD_T 15.0
#define A_VALUE_NA -1000.0

class WeatherFeed
{
public:
    WeatherFeed();
    virtual ~WeatherFeed();

    void getNMEA(std::string&);
    void writeNMEA(const std::string&);
    bool isValid();
    double getPress();
    double getTemp();

private:
    std::mutex mutex;
    std::string nmea_str;
    // hpa
    double pressure = ICAO_STD_A;
    // celsius
    double temperature = ICAO_STD_T;
    bool wind_valid = false;
};

#endif /* WINDFEED_H_ */
