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
/// A parser for SBS sentences
class CSbsParser : public IParser<object::CAircraft>
{
    CTCONST SBS_FIELD_ID   = 4;   ///< Field number of aircraft id
    CTCONST SBS_FIELD_TIME = 7;   ///< Field number of time
    CTCONST SBS_FIELD_ALT  = 11;  ///< Field number of altitude
    CTCONST SBS_FIELD_LAT  = 14;  ///< Field number of latitude
    CTCONST SBS_FIELD_LON  = 15;  ///< Field number of longitude

    s32 const m_maxHeight;  ///< The max height filter

public:
    explicit CSbsParser(s32 maxHeight_);

    auto Parse(String&& str_, u32 prio_) const -> object::CAircraft override;
};
}  // namespace vfrb::feed::parser
