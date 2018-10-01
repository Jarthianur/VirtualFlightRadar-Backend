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

#include "../../object/GpsPosition.h"

#include "Processor.hpp"

/// @namespace data
namespace data
{
/// @namespace processor
namespace processor
{
/**
 * @class GpsProcessor
 * @brief Process GPS positions to NMEA GGA and RMC sentences.
 */
class GpsProcessor : public Processor<object::GpsPosition>
{
public:
    GpsProcessor();

    ~GpsProcessor() noexcept;

    /**
     * @fn process
     * @brief Process a GPS position.
     * @param rPosition The position
     */
    void process(object::GpsPosition& position) override;

private:
    /**
     * @fn genGpggaStr
     * @brief Generate a GPGGA sentence.
     * @note Contains trailing <cr><lf>.
     * @param crPosition The position
     * @return the GPGGA sentence
     */
    std::string get_GPGGA(const object::GpsPosition& position, const std::tm* utc);

    /**
     * @fn genGprmcStr
     * @brief Generate a GPRMC sentence.
     * @note Contains trailing <cr><lf>.
     * @param crPosition The position
     * @return the GPRMC sentence
     */
    std::string get_GPRMC(const object::GpsPosition& position, const std::tm* utc);

    /**
     * @fn evalPosition
     * @brief Evaluate given latitude and longitude.
     * @param vLatitude  The latitude
     * @param vLongitude The longitude
     */
    void evalPosition(double latitude, double longitude);

    /// @var mtLatStr
    /// Orientation of the latitude (S,N)
    mutable char m_directionSN = 'x';

    /// @var mtLonStr
    /// Orientation of the longitude (E,W)
    mutable char m_directionEW = 'x';

    /// @var mtLatDeg
    /// Degrees of latitude
    mutable double m_degLatitude = 0.0;

    /// @var mtLonDeg
    /// Degrees of longitude
    mutable double m_degLongitude = 0.0;

    /// @var mtLatMin
    /// Minutes of latitude
    mutable double m_minLatitude = 0.0;

    /// @var mtLonMin
    /// Minutes of longitude
    mutable double m_minLongitude = 0.0;
};

}  // namespace processor
}  // namespace data
