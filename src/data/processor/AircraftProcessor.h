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

namespace data
{
/// @namespace aircraft
namespace processor
{
/**
 * @class AircraftProcessor
 * @brief Process Aircrafts relative to the base position.
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
    explicit AircraftProcessor(std::int32_t vMaxDist);

    /**
     * @fn ~AircraftProcessor
     * @brief Destructor
     */
    virtual ~AircraftProcessor() noexcept;

    /**
     * @fn process
     * @brief Generate NMEA report for an Aircraft with relative position etc.
     * @note Resulting string has trailing <cr><lf>.
     * @param crAircraft The Aircraft to process
     * @param crRelPos   The position to relate
     * @param vAtmPress  The atmospheric pressure
     * @param vMaxDist   The max distance
     * @return the NMEA string
     */
    std::string process(const object::Aircraft& crAircraft) override;

    void setRefered(const object::GpsPosition& crRefPosition, double vAtmPress);

private:
    /**
     * @fn calcRelPosToBase
     * @brief Calcutale an Aircrafts position relative to the base.
     * @param crAircraft The Aircraft to calculate for
     * @param crRelPos   The position to relate
     * @param vAtmPress  The atmospheric pressure
     */
    void calcRelativePosition(const object::Aircraft& crAircraft);

    /**
     * @fn buildPflauStr
     * @brief Build PFLAU sentence for an aircraft.
     * @param crAircraft The Aircaft
     * @param rDestStr   The destination string
     */
    std::string genPflauStr(const object::Aircraft& crAircraft);

    /**
     * @fn buildPflaaStr
     * @brief Build PFLAA sentence for an aircraft.
     * @param crAircraft The Aircaft
     * @param rDestStr   The destination string
     */
    std::string genPflaaStr(const object::Aircraft& crAircraft);

    /// @var mMaxDistance
    /// Max distance to process an aircraft
    const std::int32_t mMaxDistance;

    object::GpsPosition mtReferedPosition{0.0, 0.0, 0};

    double mtAtmPressure = 1013.25;

    /// @var mtRadLatB
    /// Base latitude as radian
    double mtReferedLatRad = 0.0;

    ///@var mtRadLatAc
    /// Aircraft latitude as radian
    double mtAircraftLatRad = 0.0;

    /// @var mtLatDist
    /// Distance/Difference between Aircrafts and bases latitude
    double mtLatDistance = 0.0;

    /// @var mtRadLongB
    /// Base longitude as radian
    double mtReferedLonRad = 0.0;

    ///@var mtRadLongAc
    /// Aircraft longitude as radian
    double mtAircraftLonRad = 0.0;

    /// @var mtLongDist
    /// Distance/Difference between Aircraft and base longitude
    double mtLonDistance = 0.0;

    /// @var mtBearingRel
    /// Relative bearing
    double mtBearingRel = 0.0;

    /// @var mtBearingAbs
    /// Absolute bearing
    double mtBearingAbs = 0.0;

    /// @var mtRelN
    /// Relative distance in northern direction; m
    std::int32_t mtRelNorth = 0;

    /// @var mtRelE
    /// Relative distance in eastern direction; m
    std::int32_t mtRelEast = 0;

    /// @var mtRelV
    /// Relative vertical distance; m
    std::int32_t mtRelVertical = 0;

    /// @var mtDist
    /// Distance between Aircraft and base; m
    std::int32_t mtDistance = 0;
};

}  // namespace aircraft
}
