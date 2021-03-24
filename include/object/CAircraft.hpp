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
class CAircraft : public CObject
{
    CTCONST ID_SIZE = 8;

public:
    DEFAULT_COPYABLE(CAircraft)
    COPY_ON_MOVE(CAircraft)

    CTCONST ID_LEN = 6;

    using IdString = CStaticString<ID_SIZE>;

    enum class ETargetType : enum_type
    {
        FLARM,
        TRANSPONDER
    };

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
        POWERED_AIRCRAFT      = 8,
        JET_AIRCRAFT          = 9,
        UFO                   = 10,
        BALLOON               = 11,
        AIRSHIP               = 12,
        UAV                   = 13,
        STATIC_OBJECT         = 15
    };

    enum class EIdType : enum_type
    {
        RANDOM = 0,
        ICAO   = 1,
        FLARM  = 2,
        OGN    = 3
    };

    struct SMovement
    {
        CTCONST MAX_GND_SPEED  = 10000.0;
        CTCONST MIN_GND_SPEED  = -10000.0;
        CTCONST MAX_HEADING    = 359.9;
        CTCONST MIN_HEADING    = 0.0;
        CTCONST MAX_CLIMB_RATE = 10000.0;
        CTCONST MIN_CLIMB_RATE = -10000.0;

        f64 GndSpeed;
        f64 Heading;
        f64 ClimbRate;
    };

private:
    IdString      m_id;
    EIdType       m_idType;
    EAircraftType m_aircraftType;
    ETargetType   m_targetType;
    SLocation     m_location;
    SMovement     m_movement;
    CTimestamp    m_timestamp;

    void
    assign(CObject&& other_) override;
    [[nodiscard]] auto
    canUpdate(CObject const& other_) const -> bool override;

public:
    CAircraft(u32 prio_, StringView const& id_, EIdType idT_, EAircraftType aT_, ETargetType tt_,
              SLocation const& loc_, SMovement const& move_, CTimestamp const& ts_);

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
};
}  // namespace vfrb::object
