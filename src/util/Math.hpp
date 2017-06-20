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

#ifndef SRC_UTIL_MATH_H_
#define SRC_UTIL_MATH_H_

#include <cstdint>
#include <cmath>
#include <cstddef>

namespace util
{
/**
 * Provide several functions and constants under this namespace Math.
 */
namespace math
{
/// Unit conversion constants
const double KTS_2_KMH = 1.852;
const double KMH_2_KTS = 0.539957;
const double KTS_2_MS = 0.514444;
const double MS_2_KTS = 1.94384;
const double KMH_2_MS = 0.277778;
const double MS_2_KMH = 3.6;
const double MS_2_FPM = 196.85;
const double FPM_2_MS = 0.00508;
const double FEET_2_M = 0.3048;
const double M_2_FEET = 3.28084;
const double PI = std::acos(-1.0);

/**
 * Convert degree to radian.
 *
 * @param deg the degrees
 *
 * @return the radian
 */
inline double radian(double deg)
{
    return ((deg * PI) / 180.0);
}

/**
 * Convert radian to degree.
 *
 * @param rad the radian
 *
 * @return the degrees
 */
inline double degree(double rad)
{
    return (rad * 180.0) / PI;
}

/**
 * Convert to int, round to nearest number.
 *
 * @param d the floating point value
 *
 * @return the rounded integer
 */
inline std::int32_t dToI(double d)
{
    return (d >= 0.0) ? (std::int32_t) (d + 0.5) : (std::int32_t) (d - 0.5);
}

/**
 * Convert : ddmm.hh ; h = 1/100 m
 * ( degree, minute-as-decimal )
 * to degree.
 * Sign is determined otherwise.
 *
 * @param dm the degree-minute value
 *
 * @return the degree value
 */
inline double dmToDeg(double dm)
{
    double absDm = std::abs(dm / 100.0);
    double d = std::floor(absDm);
    double m = (absDm - d) * 100.0 / 60.0;
    return d + m;
}

/**
 * Calculate height difference from QNE to Pressure in meters
 * with ICAO height formula.
 *
 * @param press the air pressure
 *
 * @return the height difference to QNE
 */
inline double calcIcaoHeight(double press)
{
    return 288.15 * (1.0 - std::pow((press / 1013.25), 0.190295)) / 0.0065;
}

/**
 * Compute checksum of nmea string.
 *
 * @param sentence the sentence to eval
 * @param size     the sentences size
 *
 * @return the checksum
 */
inline std::int32_t checksum(const char* sentence, std::size_t size)
{
    std::int32_t csum = 0;
    std::size_t i = 1; // $ in nmea str not included
    while (sentence[i] != '*' && sentence[i] != '\0' && i < size)
    {
        csum ^= (std::int32_t) sentence[i++];
    }
    return csum;
}

}  // namespace math
}  // namespace util

#endif /* SRC_UTIL_MATH_H_ */
