/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "util/ClassUtils.hpp"

#include "CGpsPosition.hpp"
#include "CObject.hpp"
#include "CStaticString.hpp"
#include "CTimestamp.hpp"
#include "Types.hpp"

namespace vfrb::object
{
/// An aircraft object
class CAircraft : public CObject
{
    CTCONST ID_SIZE = 8;  ///< Size allocated for ID

public:
    DEFAULT_COPYABLE(CAircraft)
    COPY_ON_MOVE(CAircraft)

    CTCONST ID_LEN = 6;  ///< Length of the ID

    /// Device type from which the information is received
    enum class ETargetType : enum_type
    {
        FLARM,
        TRANSPONDER
    };

    /// Aircraft types with their protocol codes
    enum class EAircraftType : enum_type
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

    /// Id (address) types with their protocol codes
    enum class EIdType : enum_type
    {
        RANDOM = 0,
        ICAO   = 1,
        FLARM  = 2,
        OGN    = 3
    };

    /// Information about an Aircrafts movement
    struct SMovement
    {
        CTCONST MAX_GND_SPEED  = 10000.0;
        CTCONST MIN_GND_SPEED  = -10000.0;
        CTCONST MAX_HEADING    = 359.9;
        CTCONST MIN_HEADING    = 0.0;
        CTCONST MAX_CLIMB_RATE = 10000.0;
        CTCONST MIN_CLIMB_RATE = -10000.0;

        f64 GndSpeed;   ///< Speed over ground; m/s
        f64 Heading;    ///< Heading; deg [0-359]
        f64 ClimbRate;  ///< Climb rate; m/s
    };

private:
    CStaticString<ID_SIZE> m_id;                ///< Aircraft identifier
    EIdType                m_idType;            ///< Id type
    EAircraftType          m_aircraftType;      ///< Aircraft type
    ETargetType            m_targetType;        ///< Target type
    SLocation              m_location;          ///< Currently known position
    SMovement              m_movement;          ///< Currently known movement
    CTimestamp             m_timestamp;         ///< The timestamp of the last report
    bool                   m_fullInfo = false;  ///< Is full set of information available?

    void
    assign(CObject&& other_) override;
    [[nodiscard]] auto
    canUpdate(CObject const& other_) const -> bool override;

public:
    /**
     * Initialize as FLARM starget.
     * @param prio_ The initial priority
     */
    CAircraft(u32 prio_, StringView const& id_, EIdType idT_, EAircraftType aT_, ETargetType tt_,
              SLocation const& loc_, SMovement const& move_, CTimestamp const& ts_);

    /**
     * Initialize as TRANSPONDER target.
     * @param prio_ The initial priority
     */
    CAircraft(u32 prio_, StringView const& id_, EIdType idT_, EAircraftType aT_, ETargetType tt_,
              SLocation const& loc_, CTimestamp const& ts_);

    ~CAircraft() noexcept override = default;

    void
    TargetType(ETargetType tt_);

    GETTER_REF(Id, m_id)
    GETTER_REF(Location, m_location)
    GETTER_REF(Movement, m_movement)
    GETTER_REF(Timestamp, m_timestamp)
    GETTER(IdType, m_idType)
    GETTER(AircraftType, m_aircraftType)
    GETTER(TargetType, m_targetType)
    GETTER(HasFullInfo, m_fullInfo)
};
}  // namespace vfrb::object
