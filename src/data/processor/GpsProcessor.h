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

#include <string>

#include "../object/Position.h"
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
    /**
     * @fn GpsModule
     * @brief Constructor
     */
    GpsProcessor();

    /**
     * @fn ~GpsModule
     * @brief Destructor
     */
    virtual ~GpsProcessor() noexcept;

    /**
     * @fn process
     * @brief Process a GPS position.
     * @param crPosition The position
     * @return the GGA and RMC sentences
     */
    std::string process(const object::GpsPosition& crPosition) override;

private:
    /**
     * @fn genGpggaStr
     * @brief Generate a GPGGA sentence.
     * @note Contains trailing <cr><lf>.
     * @param crPosition The position
     * @return the GPGGA sentence
     */
    std::string genGpggaStr(const object::GpsPosition& crPosition);

    /**
     * @fn genGprmcStr
     * @brief Generate a GPRMC sentence.
     * @note Contains trailing <cr><lf>.
     * @param crPosition The position
     * @return the GPRMC sentence
     */
    std::string genGprmcStr(const object::GpsPosition& crPosition);

    /**
     * @fn evalPosition
     * @brief Evaluate given latitude and longitude.
     * @param vLatitude  The latitude
     * @param vLongitude The longitude
     */
    void evalPosition(double vLatitude, double vLongitude);

    /// @var mtLatStr
    /// Orientation of the latitude (S,N)
    char mtLatStr = 'x';

    /// @var mtLonStr
    /// Orientation of the longitude (E,W)
    char mtLonStr = 'x';

    /// @var mtLatDeg
    /// Degrees of latitude
    double mtLatDeg = 0.0;

    /// @var mtLonDeg
    /// Degrees of longitude
    double mtLonDeg = 0.0;

    /// @var mtLatMin
    /// Minutes of latitude
    double mtLatMin = 0.0;

    /// @var mtLonMin
    /// Minutes of longitude
    double mtLonMin = 0.0;
};

}  // namespace processor
}  // namespace data
