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

#include <regex>

#include "object/CAircraft.hpp"
#include "util/ClassUtils.hpp"

#include "IParser.hpp"

namespace vfrb::feed::parser
{
class CAprsParser : public IParser<object::CAircraft>
{
    using AircraftInfo = Tuple<StringView, object::CAircraft::EIdType, object::CAircraft::EAircraftType>;

    CTCONST RE_APRS_TIME    = 1;
    CTCONST RE_APRS_LAT     = 2;
    CTCONST RE_APRS_LAT_DIR = 3;
    CTCONST RE_APRS_LON     = 4;
    CTCONST RE_APRS_LON_DIR = 5;
    CTCONST RE_APRS_HEAD    = 6;
    CTCONST RE_APRS_GND_SPD = 7;
    CTCONST RE_APRS_ALT     = 8;
    CTCONST RE_APRS_TYPE    = 9;
    CTCONST RE_APRS_ID      = 10;
    CTCONST RE_APRS_CR      = 11;
    CTCONST RE_APRS_TR      = 12;

    CTCONST CLIMB_RATE_MIN     = -10000.0;
    CTCONST CLIMB_RATE_MAX     = 10000.0;
    CTCONST ID_TYPE_BITS       = 0x03U;
    CTCONST AIRCRAFT_TYPE_BITS = 0x7CU;

    std::regex const m_aprsRe;
    i32 const        m_maxHeight;

    [[nodiscard]] auto static parseLocation(std::cmatch const& match_, i32 maxHeight_) -> object::SLocation;

    [[nodiscard]] auto static parseComment(std::cmatch const& match_) -> AircraftInfo;

    [[nodiscard]] auto static parseMovement(std::cmatch const& match_)
        -> Optional<object::CAircraft::SMovement>;

    [[nodiscard]] auto static parseTimeStamp(std::cmatch const& match_) -> object::CTimestamp;

public:
    explicit CAprsParser(i32 maxHeight_);

    auto
    Parse(String&& str_, u32 prio_) const -> object::CAircraft override;
};
}  // namespace vfrb::feed::parser
