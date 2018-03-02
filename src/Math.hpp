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

#include <cmath>
#include <cstddef>
#include <cstdint>

/**
 * @brief Provide several functions and constants under this namespace.
 */
namespace math
{
/// Unit conversion constants
const double KTS_2_KMH = 1.852;
const double KMH_2_KTS = 0.539957;
const double KTS_2_MS  = 0.514444;
const double MS_2_KTS  = 1.94384;
const double KMH_2_MS  = 0.277778;
const double MS_2_KMH  = 3.6;
const double MS_2_FPM  = 196.85;
const double FPM_2_MS  = 0.00508;
const double FEET_2_M  = 0.3048;
const double M_2_FEET  = 3.28084;
const double PI        = std::acos(-1.0);

/**
 * @fn radian
 * @brief Convert degree to radian.
 * @param deg The degrees
 * @return the radian
 */
inline double radian(double deg)
{
    return ((deg * PI) / 180.0);
}

/**
 * @fn degree
 * @brief Convert radian to degree.
 * @param rad The radian
 * @return the degrees
 */
inline double degree(double rad)
{
    return (rad * 180.0) / PI;
}

/**
 * @fn dToI
 * @brief Convert double to int, round to nearest number.
 * @param d The floating point value
 * @return the rounded integer
 */
inline std::int32_t doubleToInt(double d)
{
    return (d >= 0.0) ? static_cast<std::int32_t>(d + 0.5)
                      : static_cast<std::int32_t>(d - 0.5);
}

/**
 * @fn dmToDeg
 * @brief Convert ( degree, minute-as-decimal ) to degree.
 * @param dm The degree-minute value
 * @return the degree value
 */
inline double dmToDeg(double dm)
{
    double absDm = std::abs(dm / 100.0);
    double d     = std::floor(absDm);
    double m     = (absDm - d) * 100.0 / 60.0;
    return d + m;
}

/**
 * @fn calcIcaoHeight
 * @brief Calculate height difference from QNE to Pressure in meters with ICAO height
 * formula.
 * @param press The air pressure
 * @return the height difference to QNE
 */
inline std::int32_t icaoHeight(double press)
{
    return doubleToInt(288.15 * (1.0 - std::pow((press / 1013.25), 0.190295)) / 0.0065);
}

/**
 * @fn checksum
 * @brief Compute checksum of nmea string.
 * @param sentence The sentence to eval
 * @param size     The sentences size
 * @return the checksum
 */
inline std::int32_t checksum(const char* sentence, std::size_t size)
{
    std::int32_t csum = 0;
    std::size_t i     = 1;  // $ in nmea str not included
    while(i < size && sentence[i] != '*' && sentence[i] != '\0')
    {
        csum ^= static_cast<std::int32_t>(sentence[i++]);
    }
    return csum;
}

}  // namespace math
