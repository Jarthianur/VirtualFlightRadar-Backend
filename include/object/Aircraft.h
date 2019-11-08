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

#include "util/CString.hpp"

#include "GpsPosition.h"
#include "Object.h"
#include "Timestamp.h"
#include "types.h"

namespace vfrb::object
{
/**
 * @brief Extend Object to an aircraft.
 */
class Aircraft : public Object
{
public:
    inline static constexpr auto ID_LEN  = 6;
    inline static constexpr auto ID_SIZE = 8;

    /**
     * @brief Device type from which the information is received.
     * @note FLARM is preferred over TRANSPONDER,
     *       in case an aircraft has both available.
     */
    enum class TargetType : enum_type
    {
        FLARM,
        TRANSPONDER
    };

    /**
     * @brief Aircraft types with their protocol codes.
     */
    enum class AircraftType : enum_type
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
    enum class IdType : enum_type
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
        inline static constexpr auto MAX_GND_SPEED  = 10000.0;
        inline static constexpr auto MIN_GND_SPEED  = -10000.0;
        inline static constexpr auto MAX_HEADING    = 359.9;
        inline static constexpr auto MIN_HEADING    = 0.0;
        inline static constexpr auto MAX_CLIMB_RATE = 10000.0;
        inline static constexpr auto MIN_CLIMB_RATE = -10000.0;

        f64 gndSpeed;   ///< Speed over ground; m/s
        f64 heading;    ///< Heading; deg [0-359]
        f64 climbRate;  ///< Climb rate; m/s
    };

private:
    util::CString<ID_SIZE> m_id;                ///< Aircraft identifier
    IdType                 m_idType;            ///< Id type
    AircraftType           m_aircraftType;      ///< Aircraft type
    TargetType             m_targetType;        ///< Target type
    Location               m_location;          ///< Currently known position.
    Movement               m_movement;          ///< Currently known movement.
    Timestamp              m_timestamp;         ///< The timestamp of the last report.
    bool                   m_fullInfo = false;  ///< Is full set of information available?

    /**
     * @brief Assign an other aircrafts values to this.
     * @param other The other Aircraft
     */
    void assign(Object&& other) override;

    /**
     * @brief Override Object::canUpdate.
     */
    bool canUpdate(Object const& other) const override;

public:
    Aircraft(u32 priority, std::string_view const& id, IdType idT, AircraftType aT, Location const& loc,
             Movement const& move, Timestamp const& timestamp);
    Aircraft(u32 priority, std::string_view const& id, IdType idT, AircraftType aT, Location const& loc,
             Timestamp const& timestamp);
    Aircraft(Aircraft&& other);
    ~Aircraft() noexcept override = default;

    Aircraft& operator=(Aircraft&& other);

    auto idType() const -> decltype(m_idType);
    auto aircraftType() const -> decltype(m_aircraftType);
    auto id() const -> decltype(m_id) const&;
    auto targetType() const -> decltype(m_targetType);
    auto location() const -> decltype(m_location) const&;
    auto movement() const -> decltype(m_movement) const&;
    auto timestamp() const -> decltype(m_timestamp) const&;
    auto hasFullInfo() const -> decltype(m_fullInfo);
    void targetType(TargetType tt);
};
}  // namespace vfrb::object
