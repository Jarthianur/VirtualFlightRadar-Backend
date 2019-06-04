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
#include <cstring>

#include "impl/DateTimeImplBoost.h"
#include "util/utility.hpp"

#include "GpsPosition.h"
#include "Object.h"
#include "Timestamp.hpp"

namespace object
{
/**
 * @brief Extend Object to an aircraft.
 */
class Aircraft : public Object
{
public:
    //< begin constants >//
    static constexpr const auto ID_SIZE   = 8;
    static constexpr const auto NMEA_SIZE = 192;
    //< end constants >//

    /**
     * @brief Device type from which the information is received.
     * @note FLARM is preferred over TRANSPONDER,
     *       in case an aircraft has both available.
     */
    enum class TargetType : std::uint_fast8_t
    {
        FLARM,
        TRANSPONDER
    };

    /**
     * @brief Aircraft types with their protocol codes.
     */
    enum class AircraftType : std::uint_fast8_t
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
    enum class IdType : std::uint_fast8_t
    {
        RANDOM = 0,
        ICAO   = 1,
        FLARM  = 2,
        OGN    = 3
    };

    /**
     * @brief Hold information about an Aircrafts movement.
     */
    struct Movement
    {
        //< begin constants >//
        static constexpr auto MAX_GND_SPEED  = 10000.0;
        static constexpr auto MIN_GND_SPEED  = -10000.0;
        static constexpr auto MAX_HEADING    = 359.9;
        static constexpr auto MIN_HEADING    = 0.0;
        static constexpr auto MAX_CLIMB_RATE = 10000.0;
        static constexpr auto MIN_CLIMB_RATE = -10000.0;
        //< end constants >//

        //< begin members >//
        double gndSpeed;   ///< Speed over ground; m/s
        double heading;    ///< Heading; deg [0-359]
        double climbRate;  ///< Climb rate; m/s
        //< end members >//
    };

private:
    //< begin members >//
    util::CString<ID_SIZE>             m_id;                ///< Aircraft identifier
    IdType                             m_idType;            ///< Id type
    AircraftType                       m_aircraftType;      ///< Aircraft type
    TargetType                         m_targetType;        ///< Target type
    Location                           m_location;          ///< Currently known position.
    Movement                           m_movement;          ///< Currently known movement.
    Timestamp<time::DateTimeImplBoost> m_timestamp;         ///< The timestamp of the last report.
    bool                               m_fullInfo = false;  ///< Is full set of information available?
    util::CString<NMEA_SIZE>           m_nmea;
    //< end members >//

    //< begin methods >//
    /**
     * @brief Assign an other aircrafts values to this.
     * @param other The other Aircraft
     */
    void assign(Object&& other) override;

    /**
     * @brief Override Object::canUpdate.
     */
    bool canUpdate(const Object& other) const override;
    //< end methods >//

public:
    Aircraft(std::uint32_t priority, const std::string& id, IdType idT, AircraftType aT, const Location& loc,
             const Movement& move, const Timestamp<time::DateTimeImplBoost>& timestamp);
    Aircraft(std::uint32_t priority, const std::string& id, IdType idT, AircraftType aT, const Location& loc,
             const Timestamp<time::DateTimeImplBoost>& timestamp);
    ~Aircraft() noexcept override = default;

    //< begin operators >//
    util::CString<NMEA_SIZE>& operator*();
    Aircraft&                 operator=(Aircraft&& other);
    //< end operators >//

    //< begin interfaces >//
    util::CStringPack getNMEA() const override;
    auto              getIdType() const -> decltype(m_idType);
    auto              getAircraftType() const -> decltype(m_aircraftType);
    auto              getId() const -> const decltype(m_id)&;
    auto              getTargetType() const -> decltype(m_targetType);
    auto              getLocation() const -> const decltype(m_location)&;
    auto              getMovement() const -> const decltype(m_movement)&;
    auto              getTimestamp() const -> const decltype(m_timestamp)&;
    auto              hasFullInfo() const -> decltype(m_fullInfo);
    void              setTargetType(TargetType tt);
    //< end interfaces >//
};

}  // namespace object
