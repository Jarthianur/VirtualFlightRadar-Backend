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

#include <cstdint>
#include <string>

#include "object/Aircraft.h"
#include "object/GpsPosition.h"
#include "util/defines.h"

#include "Processor.hpp"

namespace data
{
namespace processor
{
/**
 * @brief Process aircrafts to NMEA relative to the refered position.
 */
class AircraftProcessor : public Processor<object::Aircraft>
{
public:
    DEFAULT_DTOR(AircraftProcessor)

    AircraftProcessor();

    /**
     * @brief Constructor
     * @param maxDist The max distance filter
     */
    explicit AircraftProcessor(std::int32_t maxDist);

    /**
     * @brief Process an aircraft.
     * @param aircraft The Aircraft to process
     */
    void process(object::Aircraft& aircraft) override;

    /**
     * @brief Set the refered position and atmospheric pressure.
     * @param position The position
     * @param atmPress The pressure
     */
    void referTo(const object::Position& position, double atmPress);

private:
    /**
     * @brief Calcutale an aircrafts position relative to the refered one.
     * @param aircraft The Aircraft
     */
    void calculateRelPosition(const object::Aircraft& aircraft);

    /**
     * @brief Append PFLAU sentence to processing string.
     * @param aircraft The Aircaft
     */
    void appendPFLAU(const object::Aircraft& aircraft);

    /**
     * @brief Append PFLAA sentence to processing string.
     * @param aircraft The Aircaft
     */
    void appendPFLAA(const object::Aircraft& aircraft);

    /// Max distance to process an aircraft
    const std::int32_t m_maxDistance;

    /// Refered position
    mutable object::Position m_refPosition{0.0, 0.0, 0};

    /// Refered pressure; hPa
    mutable double m_refAtmPressure = 1013.25;

    /// Refered latitude as radian
    mutable double m_refRadLatitude = 0.0;

    /// Aircraft latitude as radian
    mutable double m_aircraftRadLatitude = 0.0;

    /// Distance/Difference between Aircraft's and refered latitude
    mutable double m_latDistance = 0.0;

    /// Refered longitude as radian
    mutable double m_refRadLongitude = 0.0;

    /// Aircraft's longitude as radian
    mutable double m_aircraftRadLongitude = 0.0;

    /// Distance/Difference between Aircraft's and refered longitude
    mutable double m_lonDistance = 0.0;

    /// Relative bearing
    mutable double m_relBearing = 0.0;

    /// Absolute bearing
    mutable double m_absBearing = 0.0;

    /// Relative distance in northern direction; m
    mutable std::int32_t m_relNorth = 0;

    /// Relative distance in eastern direction; m
    mutable std::int32_t m_relEast = 0;

    /// Relative vertical distance; m
    mutable std::int32_t m_relVertical = 0;

    /// Distance between Aircraft and refered position; m
    mutable std::int32_t m_distance = 0;
};

}  // namespace processor
}  // namespace data
