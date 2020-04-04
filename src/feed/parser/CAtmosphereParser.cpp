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

#include "feed/parser/CAtmosphereParser.hpp"

#include "util/string_utils.hpp"

using namespace vfrb::object;
using namespace vfrb::str_util;

namespace vfrb::feed::parser
{
CAtmosphere CAtmosphereParser::Parse(String&& str_, u32 prio_) const {
    try {
        if (MatchChecksum({str_.c_str(), str_.length()}) && str_.find("MDA") != String::npos) {
            usize tmpB;
            if ((tmpB = str_.find('B')) != String::npos) {
                --tmpB;
            } else {
                throw error::CParseError();
            }
            usize tmpS;
            if ((tmpS = StringView(str_.c_str(), tmpB).find_last_of(',')) != String::npos) {
                ++tmpS;
            } else {
                throw error::CParseError();
            }
            if (auto [v, ec] = Convert<f64>(str_.c_str() + tmpS, str_.c_str() + tmpB); ec == EErrc::OK) {
                return {prio_, v * 1000.0, std::move(str_)};
            }
        }
    } catch ([[maybe_unused]] str_util::error::CConversionError const&) {
    }
    throw error::CParseError();
}
}  // namespace vfrb::feed::parser
