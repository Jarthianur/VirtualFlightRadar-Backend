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

#include <ctime>
#include <string>

#include "data/processor/Processor.hpp"
#include "object/GpsPosition.h"
#include "util/defines.h"

namespace data
{
namespace processor
{
/**
 * @brief Process GPS positions to NMEA GGA and RMC sentences.
 */
class GpsProcessor : public Processor<object::GpsPosition>
{
public:
    DEFAULT_DTOR(GpsProcessor)

    GpsProcessor();

    /**
     * @brief Process a GPS position.
     * @param rPosition The position
     */
    void process(object::GpsPosition& position) override;

private:
    /**
     * @brief Append GPGGA sentence to processing string.
     * @param position The position
     * @param utc      The current utc time
     */
    void appendGPGGA(const object::GpsPosition& position, const std::tm* utc);

    /**
     * @brief Append GPRMC sentence to processing string.
     * @param utc The current utc time
     */
    void appendGPRMC(const std::tm* utc);

    /**
     * @brief Evaluate position for given latitude and longitude.
     * @param latitude  The latitude
     * @param longitude The longitude
     */
    void evalPosition(double latitude, double longitude);

    /// Orientation of the latitude (S,N)
    mutable char m_directionSN = 'x';

    /// Orientation of the longitude (E,W)
    mutable char m_directionEW = 'x';

    /// Degrees of latitude
    mutable double m_degLatitude = 0.0;

    /// Degrees of longitude
    mutable double m_degLongitude = 0.0;

    /// Minutes of latitude
    mutable double m_minLatitude = 0.0;

    /// Minutes of longitude
    mutable double m_minLongitude = 0.0;
};

}  // namespace processor
}  // namespace data
