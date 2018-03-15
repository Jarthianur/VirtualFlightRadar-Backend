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

#include "../object/Aircraft.h"
#include "../object/Position.h"
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
    /**
     * @fn AircraftProcessor
     * @brief Constructor
     */
    AircraftProcessor();

    /**
     * @fn AircraftProcessor
     * @brief Constructor
     * @param vMaxDist The max distance filter
     */
    explicit AircraftProcessor(std::int32_t vMaxDist);

    /**
     * @fn ~AircraftProcessor
     * @brief Destructor
     */
    virtual ~AircraftProcessor() noexcept;

    /**
     * @fn process
     * @brief Generate NMEA report for an Aircraft.
     * @note Resulting string has trailing <cr><lf>.
     * @param crAircraft The Aircraft to process
     * @return the NMEA string
     */
    std::string process(const object::Aircraft& crAircraft) override;

    /**
     * @fn setRefered
     * @brief Set the refered position and atmospheric pressure.
     * @note This method must be called before process, in order to process an Aircraft
     * for the latest state.
     * @param crRefPosition The position
     * @param vAtmPress     The pressure
     */
    void setRefered(const object::Position& crRefPosition, double vAtmPress);

private:
    /**
     * @fn calcRelativePosition
     * @brief Calcutale an Aircrafts position relative to the refered one.
     * @param crAircraft The Aircraft to calculate for
     */
    void calcRelativePosition(const object::Aircraft& crAircraft);

    /**
     * @fn genPflauStr
     * @brief Generate PFLAU sentence for an Aircraft.
     * @param crAircraft The Aircaft
     * @return the sentence
     */
    std::string genPflauStr(const object::Aircraft& crAircraft);

    /**
     * @fn genPflaaStr
     * @brief Generate PFLAA sentence for an Aircraft.
     * @param crAircraft The Aircaft
     * @return the sentence
     */
    std::string genPflaaStr(const object::Aircraft& crAircraft);

    /// @var mMaxDistance
    /// Max distance to process an aircraft
    const std::int32_t mMaxDistance;

    /// @var mtReferedPosition
    /// The refered position
    object::Position mtReferedPosition{0.0, 0.0, 0};

    double mtAtmPressure = 1013.25;

    /// @var mtReferedLatRad
    /// Refered latitude as radian
    double mtReferedLatRad = 0.0;

    /// @var mtAircraftLatRad
    /// Aircraft latitude as radian
    double mtAircraftLatRad = 0.0;

    /// @var mtLatDistance
    /// Distance/Difference between Aircraft's and refered latitude
    double mtLatDistance = 0.0;

    /// @var mtReferedLonRad
    /// Refered longitude as radian
    double mtReferedLonRad = 0.0;

    /// @var mtAircraftLonRad
    /// Aircraft's longitude as radian
    double mtAircraftLonRad = 0.0;

    /// @var mtLonDistance
    /// Distance/Difference between Aircraft's and refered longitude
    double mtLonDistance = 0.0;

    /// @var mtBearingRel
    /// Relative bearing
    double mtBearingRel = 0.0;

    /// @var mtBearingAbs
    /// Absolute bearing
    double mtBearingAbs = 0.0;

    /// @var mtRelNorth
    /// Relative distance in northern direction; m
    std::int32_t mtRelNorth = 0;

    /// @var mtRelEast
    /// Relative distance in eastern direction; m
    std::int32_t mtRelEast = 0;

    /// @var mtRelVertical
    /// Relative vertical distance; m
    std::int32_t mtRelVertical = 0;

    /// @var mtDistance
    /// Distance between Aircraft and refered position; m
    std::int32_t mtDistance = 0;
};

}  // namespace processor
}  // namespace data
