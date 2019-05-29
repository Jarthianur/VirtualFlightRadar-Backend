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
#include "TimeStamp.hpp"

namespace data
{
class AircraftData;
namespace processor
{
class AircraftProcessor;
}  // namespace processor
}  // namespace data
namespace feed
{
namespace parser
{
class SbsParser;
class AprsParser;
}  // namespace parser
}  // namespace feed

namespace object
{
/**
 * @brief Extend Object to an aircraft.
 */
class Aircraft : public Object
{
    friend class data::AircraftData;
    friend class data::processor::AircraftProcessor;
    friend class feed::parser::AprsParser;
    friend class feed::parser::SbsParser;

    //< begin constants >//
    static constexpr const auto VALUE_NA  = -1024.0;
    static constexpr const auto ID_SIZE   = 8;
    static constexpr const auto NMEA_SIZE = 4096;
    //< end constants >//

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
     * @brief Hold information about an Aircrafts movement.
     */
    struct Movement
    {
        //< begin members >//
        double gndSpeed  = VALUE_NA;  ///< Speed over ground; m/s
        double heading   = VALUE_NA;  ///< Heading; deg [0-359]
        double climbRate = VALUE_NA;  ///< Climb rate; m/s
        //< end members >//
    };

    //< begin members >//
    IdType                             m_idType;                 ///< Id type
    AircraftType                       m_aircraftType;           ///< Aircraft type
    util::CString<ID_SIZE>             m_id;                     ///< Aircraft identifier
    TargetType                         m_targetType;             ///< Target type
    Location                           m_position{0.0, 0.0, 0};  ///< Currently known position.
    Movement                           m_movement;               ///< Currently known movement.
    TimeStamp<time::DateTimeImplBoost> m_timeStamp;              ///< The timestamp of the last report.
    bool                               m_fullInfo = false;       ///< Is full set of information available?
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

    /**
     * @brief Set the aircraft type.
     *
     * The type is set to UNKNOWN, if the new type value is not in range of AircraftType.
     *
     * @param type The type
     */
    void setAircraftType(AircraftType type);

    /**
     * @brief Set the id type.
     *
     * The id type is set to RANDOM, if the new type value is not in range of
     * IdType.
     *
     * @param type The type
     */
    void setIdType(IdType type);
    //< end methods >//

public:
    Aircraft();
    explicit Aircraft(std::uint32_t priority);  ///< @param priority The initial priority
    ~Aircraft() noexcept override = default;

    //< begin interfaces >//
    util::CStringPack getNMEA() const override;
    //< end interfaces >//
};

}  // namespace object
