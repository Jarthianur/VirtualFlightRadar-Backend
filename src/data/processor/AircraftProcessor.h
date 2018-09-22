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

#include "../../object/Aircraft.h"
#include "../../object/GpsPosition.h"

#include "Processor.hpp"

/// @namespace data
namespace data
{
/// @namespace processor
namespace processor
{
/**
 * @class AircraftProcessor
 * @brief Process Aircrafts relative to the refered position.
 * @extends Processor
 */
class AircraftProcessor : public Processor<object::Aircraft>
{
public:
    AircraftProcessor();

    /**
     * @fn AircraftProcessor
     * @brief Constructor
     * @param vMaxDist The max distance filter
     */
    explicit AircraftProcessor(std::int32_t maxDist);

    ~AircraftProcessor() noexcept;

    /**
     * @fn process
     * @brief Generate NMEA report for an Aircraft.
     * @note Resulting string has trailing <cr><lf>.
     * @param rAircraft The Aircraft to process
     */
    void process(object::Aircraft& aircraft) override;

    /**
     * @fn setRefered
     * @brief Set the refered position and atmospheric pressure.
     * @note This method must be called before process, in order to process an Aircraft
     * for the latest state.
     * @param crRefPosition The position
     * @param vAtmPress     The pressure
     */
    void referTo(const object::Position& position, double atmPress);

private:
    /**
     * @fn calcRelativePosition
     * @brief Calcutale an Aircrafts position relative to the refered one.
     * @param crAircraft The Aircraft to calculate for
     */
    void calculateRelPosition(const object::Aircraft& aircraft);

    /**
     * @fn genPflauStr
     * @brief Generate PFLAU sentence for an Aircraft.
     * @param crAircraft The Aircaft
     * @return the sentence
     */
    std::string get_PFLAU(const object::Aircraft& aircraft);

    /**
     * @fn genPflaaStr
     * @brief Generate PFLAA sentence for an Aircraft.
     * @param crAircraft The Aircaft
     * @return the sentence
     */
    std::string get_PFLAA(const object::Aircraft& aircraft);

    /// @var m_maxDistance
    /// Max distance to process an aircraft
    const std::int32_t m_maxDistance;

    /// @var m_refPosition
    /// The refered position
    mutable object::Position m_refPosition{0.0, 0.0, 0};

    mutable double m_refAtmPressure = 1013.25;

    /// @var mtReferedLatRad
    /// Refered latitude as radian
    mutable double m_refRadLatitude = 0.0;

    /// @var mtAircraftLatRad
    /// Aircraft latitude as radian
    mutable double m_aircraftRadLatitude = 0.0;

    /// @var m_latDistance
    /// Distance/Difference between Aircraft's and refered latitude
    mutable double m_latDistance = 0.0;

    /// @var mtReferedLonRad
    /// Refered longitude as radian
    mutable double m_refRadLongitude = 0.0;

    /// @var mtAircraftLonRad
    /// Aircraft's longitude as radian
    mutable double m_aircraftRadLongitude = 0.0;

    /// @var m_lonDistance
    /// Distance/Difference between Aircraft's and refered longitude
    mutable double m_lonDistance = 0.0;

    /// @var mtBearingRel
    /// Relative bearing
    mutable double m_relBearing = 0.0;

    /// @var mtBearingAbs
    /// Absolute bearing
    mutable double m_absBearing = 0.0;

    /// @var mtRelNorth
    /// Relative distance in northern direction; m
    mutable std::int32_t m_relNorth = 0;

    /// @var mtRelEast
    /// Relative distance in eastern direction; m
    mutable std::int32_t m_relEast = 0;

    /// @var mtRelVertical
    /// Relative vertical distance; m
    mutable std::int32_t m_relVertical = 0;

    /// @var m_distance
    /// Distance between Aircraft and refered position; m
    mutable std::int32_t m_distance = 0;
};

}  // namespace processor
}  // namespace data
