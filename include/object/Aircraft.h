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

#include "util/defines.h"

#include "DateTimeImplBoost.h"
#include "GpsPosition.h"
#include "Object.h"
#include "TimeStamp.hpp"

/// Indicate a value is not available.
#define A_VALUE_NA -1024.0

namespace object
{
/**
 * @brief Hold information about an Aircrafts movement.
 */
struct Movement
{
    /// Speed over ground; m/s
    double gndSpeed = A_VALUE_NA;

    /// Heading; deg [0-359]
    double heading = A_VALUE_NA;

    /// Climb rate; m/s
    double climbRate = A_VALUE_NA;
};

/**
 * @brief Extend Object to an aircraft.
 */
class Aircraft : public Object
{
public:
    /**
     * @brief Device type from which the information is received.
     * @note FLARM is preferred over TRANSPONDER,
     *       in case an aircraft has both available.
     */
    enum class TargetType : std::uint8_t
    {
        FLARM,
        TRANSPONDER
    };

    /**
     * @brief Aircraft types with their protocol codes.
     */
    enum class AircraftType : std::uint8_t
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
     * @brief Id (address) types with their protocol codes.
     */
    enum class IdType : std::uint8_t
    {
        RANDOM = 0,
        ICAO   = 1,
        FLARM  = 2,
        OGN    = 3
    };

    /**
     * @brief Constructor
     */
    Aircraft();

    /**
     * @brief Constructor
     * @param priority The initial priority
     */
    explicit Aircraft(std::uint32_t priority);

    /**
     * @brief Destructor
     */
    ~Aircraft() noexcept = default;

    /**
     * @brief Set the aircraft type.
     *
     * The type is set to UNKNOWN, if the new type value is not in range of AircraftType.
     *
     * @param type The type
     */
    void set_aircraftType(AircraftType type);

    /**
     * @brief Set the id type.
     *
     * The id type is set to RANDOM, if the new type value is not in range of
     * IdType.
     *
     * @param type The type
     */
    void set_idType(IdType type);

private:
    /**
     * @brief Assign an other aircrafts values to this.
     * @param other The other Aircraft
     */
    void assign(Object&& other) override;

    /**
     * @brief Override Object::canUpdate.
     */
    bool canUpdate(const Object& other) const override;

    /// Aircraft identifier
    std::string m_id;

    /// Id type
    IdType m_idType;

    /// Aircraft type
    AircraftType m_aircraftType;

    /// Target type
    TargetType m_targetType;

    /// Currently known position.
    Position m_position{0.0, 0.0, 0};

    /// Currently known movement.
    Movement m_movement;

    /// The timestamp of the last report.
    TimeStamp<timestamp::DateTimeImplBoost> m_timeStamp;

    /// Is full set of information available?
    bool m_fullInfo = false;

public:
    /**
     * Getters and setters
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
