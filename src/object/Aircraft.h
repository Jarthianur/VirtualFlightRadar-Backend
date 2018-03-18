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
#include "Object.h"
#include "GpsPosition.h"

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
    enum class TargetType : std::uint32_t
    {
        FLARM,
        TRANSPONDER
    };

    /**
     * @enum AircraftType
     * @brief Aircraft types with their protocol codes.
     */
    enum class AircraftType : std::uint32_t
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

    DEFAULT_CTOR_DTOR(Aircraft)

    /**
     * @fn Aircraft
     * @brief Constructor
     * @param vPriority The initial priority
     */
    explicit Aircraft(std::uint32_t vPriority);

    /**
     * Define and declare getters and setters.
     */
    GETSET_CR(std::string, mId, Id)
    GETTER_V(IdType, mIdType, IdType)
    GETSET_V(TargetType, mTargetType, TargetType)
    GETTER_V(AircraftType, mAircraftType, AircraftType)
    GETSET_V(bool, mFullInfo, FullInfoAvailable)
    GETTER_R(std::uint32_t, mUpdateAge, UpdateAge)
    GETSET_CR(Position, mPosition, Position)
    GETSET_CR(Movement, mMovement, Movement)

    /**
     * @fn setAircraftType
     * @brief Set the aircraft type.
     *
     * The type is set to UNKNOWN, if the new type value is not in range of AircraftType.
     *
     * @param vType The type
     */
    void setAircraftType(AircraftType vType);

    /**
     * @fn setIdType
     * @brief Set the id type.
     *
     * The id type is set to UNRECOGNIZED, if the new type value is not in range of
     * IdType.
     *
     * @param vType The type
     */
    void setIdType(IdType vType);

private:
    /**
     * @fn assign
     * @brief Assign an other Aircrafts values to this.
     * @param rvOther The other Aircraft
     */
    void assign(Object&& rvOther) override;

    /**
     * @fn canUpdate
     * @brief Check whether this Aircraft can update the other one.
     * @param crOther   The other Aircraft
     * @param vAttempts The update attempt count
     * @return true if yes, else false
     */
    bool canUpdate(const Object& crOther, std::uint32_t vAttempts) const override;

    /// @var mId
    /// Aircraft identifier
    std::string mId;

    /// @var mIdType
    /// @see IdType
    IdType mIdType;

    /// @var mAircraftType
    /// @see AircraftType
    AircraftType mAircraftType;

    /// @var mTargetType
    /// @see TargetType
    TargetType mTargetType;

    /// @var mPosition
    /// Currently known position.
    Position mPosition{0.0, 0.0, 0};

    /// @var mMovement
    /// Currently known movement.
    Movement mMovement;

    /// @var mFullInfo
    /// Is full set of information available?
    bool mFullInfo = false;

    /// @var mUpdateAge
    /// Times processed without update.
    std::uint32_t mUpdateAge = 0;
};

}  // namespace object
