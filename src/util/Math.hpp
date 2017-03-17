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

#ifndef MATH_H_
#define MATH_H_

#include <cstdint>
#include <cmath>
#include <cstddef>

namespace Math
{

/**
 * constants
 */
const double kts2kmh = 1.852;
const double kmh2kts = 0.539957;
const double kts2ms = 0.514444;
const double ms2kts = 1.94384;
const double kmh2ms = 0.277778;
const double ms2kmh = 3.6;
const double ms2fpm = 196.85;
const double fpm2ms = 0.00508;
const double feet2m = 0.3048;
const double m2feet = 3.28084;
const double PI = std::acos(-1.0);

/**
 * convert degree to radian
 */
inline double radian(double deg)
{
    return ((deg * PI) / 180.0);
}

/**
 * convert radian to degree
 */
inline double degree(double rad)
{
    return (rad * 180.0) / PI;
}

/**
 * convert to int, round to nearest number
 */
inline std::int32_t dToI(double d)
{
    return (d >= 0.0) ? (std::int32_t) (d + 0.5) : (std::int32_t) (d - 0.5);
}

/**
 * convert
 * ( degree, minute, second )
 * to degree
 * Used for APRS.
 * Sign is determined otherwise for this protocol.
 */
inline double dmsToDeg(double dms)
{
    double absDms = std::abs(dms);
    double d = std::floor(absDms);
    double t = (absDms - d) * 100.0;
    double m = std::floor(t);
    // deg + min/60 + sec/3600
    return d + (m / 60.0) + ((t - m) * 100.0) / 3600.0;
}

/**
 * calculate height difference from QNE to Pressure in meters
 * with ICAO height formula
 */
inline double calcIcaoHeight(double press)
{
    return 288.15 * (1.0 - std::pow((press / 1013.25), 0.190295)) / 0.0065;
}

/**
 * compute checksum of nmea string
 */
inline std::int32_t checksum(const char* sentence, std::size_t size)
{
    std::int32_t csum = 0;
    std::size_t i = 1;// $ in nmea str not included
    while (sentence[i] != '*' && sentence[i] != '\0' && i < size)
    {
        csum ^= (std::int32_t) sentence[i++];
    }
    return csum;
}

}

#endif /* MATH_H_ */
