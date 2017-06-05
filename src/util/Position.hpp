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

#ifndef UTIL_POSITION_HPP_
#define UTIL_POSITION_HPP_

#include <cstdint>

namespace util
{

/**
 * GPS position structure.
 *
 * Minimal information for a position.
 */
struct GPSPosition
{
    /// Altitude; m
    std::int32_t altitude;
    /// Latitude; deg
    double latitude,
    /// Longitude; deg
            longitude;
};

/**
 * Extended GPS position structure.
 *
 * Holds GPS meta information additionally to a position.
 */
struct ExtGPSPosition
{
    /// Position
    struct GPSPosition position;
    /// Number of satellites
    std::int32_t nrSats,
    /// GPS fix quality
            fixQa;
    /// Geoid separation
    double geoid;
};

}  // namespace util

#endif /* UTIL_POSITION_HPP_ */