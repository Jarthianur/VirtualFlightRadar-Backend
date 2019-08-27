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

#include "object/Aircraft.h"
#include "object/GpsPosition.h"
#include "util/types.h"

#include "Processor.hpp"

namespace data::processor
{
/**
 * @brief Process aircrafts to NMEA relative to the refered position.
 */
class AircraftProcessor : public Processor<object::Aircraft>
{
    s32 const        m_maxDistance;               ///< Max distance to process an aircraft
    object::Location m_refLocation{0.0, 0.0, 0};  ///< Refered position
    f64              m_refAtmPressure = 1013.25;  ///< Refered pressure; hPa
    f64 mutable m_refRadLatitude      = 0.0;      ///< Refered latitude as radian
    f64 mutable m_aircraftRadLatitude = 0.0;      ///< Aircraft latitude as radian
    f64 mutable m_latDistance         = 0.0;  ///< Distance/Difference between Aircraft's and refered latitude
    f64 mutable m_refRadLongitude     = 0.0;  ///< Refered longitude as radian
    f64 mutable m_aircraftRadLongitude = 0.0;  ///< Aircraft's longitude as radian
    f64 mutable m_lonDistance = 0.0;  ///< Distance/Difference between Aircraft's and refered longitude
    f64 mutable m_relBearing  = 0.0;  ///< Relative bearing
    f64 mutable m_absBearing  = 0.0;  ///< Absolute bearing
    s32 mutable m_relNorth    = 0;    ///< Relative distance in northern direction; m
    s32 mutable m_relEast     = 0;    ///< Relative distance in eastern direction; m
    s32 mutable m_relVertical = 0;    ///< Relative vertical distance; m
    s32 mutable m_distance    = 0;    ///< Distance between Aircraft and refered position; m

    /**
     * @brief Calcutale an aircrafts position relative to the refered one.
     * @param aircraft The Aircraft
     */
    void calculateRelPosition(object::Aircraft const& aircraft) const;

    /**
     * @brief Append PFLAU sentence to processing string.
     * @param aircraft The Aircaft
     */
    usize appendPFLAU(object::Aircraft& aircraft, usize pos) const;

    /**
     * @brief Append PFLAA sentence to processing string.
     * @param aircraft The Aircaft
     */
    usize appendPFLAA(object::Aircraft& aircraft, usize pos) const;

public:
    AircraftProcessor();
    explicit AircraftProcessor(s32 maxDist);  ///< @param maxDist The max distance filter
    ~AircraftProcessor() noexcept override = default;

    /**
     * @brief Process an aircraft.
     * @param aircraft The Aircraft to process
     */
    void process(object::Aircraft& aircraft) const override;

    /**
     * @brief Set the refered position and atmospheric pressure.
     * @param position The position
     * @param atmPress The pressure
     */
    void referTo(object::Location const& location, f64 atmPress);
};
}  // namespace data::processor
