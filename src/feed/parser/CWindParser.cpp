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

#include "feed/parser/CWindParser.hpp"

#include "object/CWind.hpp"
#include "util/StringUtils.hpp"

using vfrb::object::CWind;
using vfrb::str_util::MatchChecksum;

namespace vfrb::feed::parser
{
CWindParser::CWindParser() : IParser<CWind>() {}

auto
CWindParser::Parse(String&& str_, u32 prio_) const -> CWind {
    if (MatchChecksum({str_.c_str(), str_.length()}) && str_.find("MWV") != String::npos) {
        return {prio_, std::move(str_)};
    }
    throw error::CParseError();
}
}  // namespace vfrb::feed::parser
