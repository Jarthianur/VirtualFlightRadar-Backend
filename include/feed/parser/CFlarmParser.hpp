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

#include "object/CAircraft.hpp"

#include "IParser.hpp"

namespace vfrb::feed::parser
{
struct SPositionUpdate
{
    object::SLocation  Location;
    object::CTimestamp Timestamp;
};

struct SMovementUpdate
{
    object::CAircraft::SMovement Movement;
    object::CTimestamp           Timestamp;
};

using FlarmResult = Pair<object::CAircraft::IdString, Variant<SPositionUpdate, SMovementUpdate>>;

class CFlarmParser : public IParser<FlarmResult>
{
    i32 const m_maxHeight;

    auto
    parsePositionReport(String&& str_) const -> Pair<object::CAircraft::IdString, SPositionUpdate>;

    auto
    parseVelocityReport(String&& str_) const -> Pair<object::CAircraft::IdString, SMovementUpdate>;

public:
    explicit CFlarmParser(i32 maxHeight_);

    auto
    Parse(String&& str_, [[maybe_unused]] u32) const -> FlarmResult override;
};
}  // namespace vfrb::feed::parser
