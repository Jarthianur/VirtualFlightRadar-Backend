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

#include "feed/parser/WindParser.h"

#include "util/string_utils.hpp"

using namespace vfrb::object;

namespace vfrb::feed::parser
{
CWindParser::CWindParser() : IParser<CWind>() {}

CWind CWindParser::Parse(Str&& str_, u32 prio_) const
{
    if (str_util::MatchChecksum({str_.c_str(), str_.length()}) && str_.find("MWV") != Str::npos)
    {
        CWind wind{prio_};
        *wind = std::move(str_);
        return wind;
    }
    throw error::CParseError();
}
}  // namespace vfrb::feed::parser
