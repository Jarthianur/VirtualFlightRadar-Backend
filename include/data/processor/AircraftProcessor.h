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

#include "types.h"

namespace vfrb::data::processor
{
/**
 * @brief Process aircrafts to NMEA relative to the refered position.
 */
class CAircraftProcessor
{
public:
    inline static constexpr auto NMEA_SIZE = 192;

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
    usize appendPflau(object::CAircraft const& aircraft_, util::CString<NMEA_SIZE>& nmea_, usize pos_) const;

    /**
     * @brief Append PFLAA sentence to processing string.
     * @param aircraft The Aircaft
     */
    usize appendPflaa(object::CAircraft const& aircraft_, util::CString<NMEA_SIZE>& nmea_, usize pos_) const;

public:
    CAircraftProcessor();
    explicit CAircraftProcessor(s32 maxDist_);  ///< @param maxDist The max distance filter
    ~CAircraftProcessor() noexcept = default;

    /**
     * @brief Process an aircraft.
     * @param aircraft The Aircraft to process
     */
    void Process(object::CAircraft const& aircraft_, util::CString<NMEA_SIZE>& nmea_) const;

    /**
     * @brief Set the refered position and atmospheric pressure.
     * @param position The position
     * @param atmPress The pressure
     */
    void ReferTo(object::SLocation const& location_, f64 atmPress_);
};
}  // namespace vfrb::data::processor
