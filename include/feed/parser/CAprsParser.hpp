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

#include <optional>
#include <regex>
#include <tuple>

#include "object/CAircraft.hpp"
#include "util/ClassUtils.hpp"

#include "IParser.hpp"

namespace vfrb::feed::parser
{
/// A parser for APRS sentences.
class CAprsParser : public IParser<object::CAircraft>
{
    using AircraftInfo = Tuple<StringView, object::CAircraft::EIdType, object::CAircraft::EAircraftType>;

    CTCONST RE_APRS_TIME    = 1;   ///< APRS regex match group of time
    CTCONST RE_APRS_LAT     = 2;   ///< APRS regex match group of latitude
    CTCONST RE_APRS_LAT_DIR = 3;   ///< APRS regex match group of latitude orientation
    CTCONST RE_APRS_LON     = 4;   ///< APRS regex match group of longitude
    CTCONST RE_APRS_LON_DIR = 5;   ///< APRS regex match group of longitude orientation
    CTCONST RE_APRS_HEAD    = 6;   ///< APRS regex match group of heading
    CTCONST RE_APRS_GND_SPD = 7;   ///< APRS regex match group of ground speed
    CTCONST RE_APRS_ALT     = 8;   ///< APRS regex match group of altitude
    CTCONST RE_APRS_TYPE    = 9;   ///< APRS regex match group of id and aircraft type
    CTCONST RE_APRS_ID      = 10;  ///< APRS regex match group of aircraft id
    CTCONST RE_APRS_CR      = 11;  ///< APRS regex match group of climb rate
    CTCONST RE_APRS_TR      = 12;  ///< APRS regex match group of turn rate

    CTCONST CLIMB_RATE_MIN     = -10000.0;
    CTCONST CLIMB_RATE_MAX     = 10000.0;
    CTCONST ID_TYPE_BITS       = 0x03U;
    CTCONST AIRCRAFT_TYPE_BITS = 0x7CU;

    std::regex const m_aprsRe;     ///< Regular expression for APRS protocol
    i32 const        m_maxHeight;  ///< The max height filter

    /**
     * @throw vfrb::feed::parser::error::CParseError
     * @throw vfrb::str_util::error::CConversionError
     */
    [[nodiscard]] auto static parseLocation(std::cmatch const& match_, i32 maxHeight_) -> object::SLocation;

    /// @throw vfrb::str_util::error::CConversionError
    [[nodiscard]] auto static parseComment(std::cmatch const& match_) -> AircraftInfo;

    /// @throw vfrb::str_util::error::CConversionError
    [[nodiscard]] auto static parseMovement(std::cmatch const& match_)
        -> std::optional<object::CAircraft::SMovement>;

    /// @throw vfrb::object::error::CTimestampParseError
    [[nodiscard]] auto static parseTimeStamp(std::cmatch const& match_) -> object::CTimestamp;

public:
    /// @param maxHeight_ The filter for max height
    explicit CAprsParser(i32 maxHeight_);

    auto
    Parse(String&& str_, u32 prio_) const -> object::CAircraft override;
};
}  // namespace vfrb::feed::parser
