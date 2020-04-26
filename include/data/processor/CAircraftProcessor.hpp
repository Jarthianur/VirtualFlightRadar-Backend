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

#include "object/CAircraft.hpp"
#include "object/CGpsPosition.hpp"

#include "types.hpp"

namespace vfrb::data::processor
{
/**
 * @brief Process aircrafts to NMEA relative to the refered position.
 */
class CAircraftProcessor
{
public:
    CTCONST NMEA_SIZE = 192;

private:
    s32 const         m_maxDistance;               ///< Max distance to process an aircraft
    object::SLocation m_refLocation{0.0, 0.0, 0};  ///< Refered position
    f64               m_refAtmPressure = 1013.25;  ///< Refered pressure; hPa
    f64 mutable m_refRadLatitude       = 0.0;      ///< Refered latitude as radian
    f64 mutable m_aircraftRadLatitude  = 0.0;      ///< Aircraft latitude as radian
    f64 mutable m_latDistance     = 0.0;  ///< Distance/Difference between Aircraft's and refered latitude
    f64 mutable m_refRadLongitude = 0.0;  ///< Refered longitude as radian
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
    void calculateRelPosition(object::CAircraft const& aircraft_) const;

    /**
     * @brief Append PFLAU sentence to processing string.
     * @param aircraft The Aircaft
     */
    usize appendPflau(object::CAircraft const& aircraft_, CStaticString<NMEA_SIZE>& nmea_, usize idx_) const;

    /**
     * @brief Append PFLAA sentence to processing string.
     * @param aircraft The Aircaft
     */
    usize appendPflaa(object::CAircraft const& aircraft_, CStaticString<NMEA_SIZE>& nmea_, usize idx_) const;

public:
    CAircraftProcessor();
    explicit CAircraftProcessor(s32 maxDist_);  ///< @param maxDist The max distance filter

    /**
     * @brief Process an aircraft.
     * @param aircraft The Aircraft to process
     */
    void Process(object::CAircraft const& aircraft_, CStaticString<NMEA_SIZE>& nmea_) const;

    /**
     * @brief Set the refered position and atmospheric pressure.
     * @param position The position
     * @param atmPress The pressure
     */
    void ReferTo(object::SLocation const& loc_, f64 press_);
};
}  // namespace vfrb::data::processor
