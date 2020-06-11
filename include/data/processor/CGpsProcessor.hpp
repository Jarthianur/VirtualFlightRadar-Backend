/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <ctime>

#include "object/CGpsPosition.hpp"

#include "CStaticString.hpp"
#include "Types.hpp"

namespace vfrb::data::processor
{
/**
 * @brief Process GPS positions to NMEA GGA and RMC sentences.
 */
class CGpsProcessor
{
public:
    CTCONST NMEA_SIZE = 192;

private:
    char mutable m_directionSN = 'x';  ///< Orientation of the latitude (S,N)
    char mutable m_directionEW = 'x';  ///< Orientation of the longitude (E,W)
    f64 mutable m_degLatitude  = 0.0;  ///< Degrees of latitude
    f64 mutable m_degLongitude = 0.0;  ///< Degrees of longitude
    f64 mutable m_minLatitude  = 0.0;  ///< Minutes of latitude
    f64 mutable m_minLongitude = 0.0;  ///< Minutes of longitude

    /**
     * @brief Append GPGGA sentence to processing string.
     * @param position The position
     * @param utc      The current utc time
     */
    auto appendGpgga(object::CGpsPosition const& pos_, CStaticString<NMEA_SIZE>* nmea_, std::tm const* utc_,
                     usize idx_) const -> usize;

    /**
     * @brief Append GPRMC sentence to processing string.
     * @param utc The current utc time
     */
    auto appendGprmc(CStaticString<NMEA_SIZE>* nmea_, std::tm const* utc_, usize idx_) const -> usize;

    /**
     * @brief Evaluate position for given latitude and longitude.
     * @param latitude  The latitude
     * @param longitude The longitude
     */
    void evalPosition(f64 lat_, f64 lon_) const;

public:
    /**
     * @brief Process a GPS position.
     * @param rPosition The position
     */
    void Process(object::CGpsPosition const& pos_, CStaticString<NMEA_SIZE>* nmea_) const;
};
}  // namespace vfrb::data::processor
