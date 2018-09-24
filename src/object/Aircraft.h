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

#include "../Defines.h"
#include "GpsPosition.h"
#include "Object.h"
#include "TimeStamp.hpp"
#include "DateTimeImplBoost.h"

/// @def A_VALUE_NA
/// Indicate a double value is not available.
#define A_VALUE_NA -1024.0

/// @namespace object
namespace object
{
/**
 * @struct Movement
 * @brief Hold information about an Aircrafts movement.
 */
struct Movement
{
    /// @var gndSpeed
    /// Speed over ground; m/s
    double gndSpeed = A_VALUE_NA;

    /// @var heading
    /// Heading; deg [0-359]
    double heading = A_VALUE_NA;

    /// @var climbRate
    /// Climb rate; m/s
    double climbRate = A_VALUE_NA;
};

/**
 * @class Aircraft
 * @brief Respresents an aircraft.
 * @extends Object
 */
class Aircraft : public Object
{
public:
    /**
     * @enum TargetType
     * @brief Device type from which the information is received.
     * @note FLARM is preferred over TRANSPONDER,
     *       in case an aircraft has both available.
     */
    enum class TargetType : std::int32_t
    {
        FLARM,
        TRANSPONDER
    };

    /**
     * @enum AircraftType
     * @brief Aircraft types with their protocol codes.
     */
    enum class AircraftType : std::int32_t
    {
        UNKNOWN               = 0,
        GLIDER                = 1,
        TOW_PLANE             = 2,
        HELICOPTER_ROTORCRAFT = 3,
        PARACHUTE             = 4,
        DROP_PLANE            = 5,
        HANG_GLIDER           = 6,
        PARA_GLIDER           = 7,
        POWERED_AIRCRAFT      = 8,  ///< Default for TRANSPONDER targets
        JET_AIRCRAFT          = 9,
        UFO                   = 10,
        BALLOON               = 11,
        AIRSHIP               = 12,
        UAV                   = 13,
        STATIC_OBJECT         = 15
    };

    /**
     * @enum IdType
     * @brief Id (address) types with their protocol codes.
     */
    enum class IdType : std::int32_t
    {
        UNRECOGNIZED = -1,
        RANDOM       = 0,
        ICAO         = 1,
        FLARM        = 2,
        OGN          = 3
    };

    Aircraft();

    /**
     * @fn Aircraft
     * @brief Constructor
     * @param vPriority The initial priority
     */
    explicit Aircraft(std::uint32_t priority);

    ~Aircraft() noexcept;

    /**
     * @fn setAircraftType
     * @brief Set the aircraft type.
     *
     * The type is set to UNKNOWN, if the new type value is not in range of AircraftType.
     *
     * @param vType The type
     */
    void set_aircraftType(AircraftType type);

    /**
     * @fn setIdType
     * @brief Set the id type.
     *
     * The id type is set to UNRECOGNIZED, if the new type value is not in range of
     * IdType.
     *
     * @param vType The type
     */
    void set_idType(IdType type);

private:
    /**
     * @fn assign
     * @brief Assign an other Aircrafts values to this.
     * @param rvOther The other Aircraft
     */
    void assign(Object&& other) override;

    /**
     * @fn canUpdate
     * @brief Check whether this Aircraft can update the other one.
     * @param crOther   The other Aircraft
     * @param vAttempts The update attempt count
     * @return true if yes, else false
     */
    bool canUpdate(const Object& other) const override;

    /// @var mId
    /// Aircraft identifier
    std::string m_id;

    /// @var mIdType
    /// @see IdType
    IdType m_idType;

    /// @var mAircraftType
    /// @see AircraftType
    AircraftType m_aircraftType;

    /// @var mTargetType
    /// @see TargetType
    TargetType m_targetType;

    /// @var mPosition
    /// Currently known position.
    Position m_position{0.0, 0.0, 0};

    /// @var mMovement
    /// Currently known movement.
    Movement m_movement;

    /// @var mTimeStamp
    /// The timestamp of the last report.
    TimeStamp<DateTimeImplBoost> m_timeStamp;

    /// @var mFullInfo
    /// Is full set of information available?
    bool m_fullInfo = false;

public:
    /**
     * Define and declare getters and setters.
     */
    GETSET_CR(id)
    GETTER_V(idType)
    GETSET_V(targetType)
    GETTER_V(aircraftType)
    GETSET_V(fullInfo)
    GETSET_CR(position)
    GETSET_CR(movement)
    GETSET_V(timeStamp)
};

}  // namespace object
