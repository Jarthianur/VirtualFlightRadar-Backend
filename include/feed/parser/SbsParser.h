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

#include "object/Aircraft.h"

#include "Parser.hpp"

namespace vfrb::feed::parser
{
/// A parser for SBS sentences
class CSbsParser : public IParser<object::CAircraft>
{
    inline static constexpr auto SBS_FIELD_ID   = 4;   ///< Field number of aircraft id
    inline static constexpr auto SBS_FIELD_TIME = 7;   ///< Field number of time
    inline static constexpr auto SBS_FIELD_ALT  = 11;  ///< Field number of altitude
    inline static constexpr auto SBS_FIELD_LAT  = 14;  ///< Field number of latitude
    inline static constexpr auto SBS_FIELD_LON  = 15;  ///< Field number of longitude

    s32 const m_maxHeight;  ///< The max height filter

public:
    explicit CSbsParser(s32 maxHeight_);
    ~CSbsParser() noexcept override = default;

    object::CAircraft Parse(Str&& str_, u32 prio_) const override;
};
}  // namespace vfrb::feed::parser
