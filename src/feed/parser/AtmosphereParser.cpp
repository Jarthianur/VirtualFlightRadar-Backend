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

#include "feed/parser/AtmosphereParser.h"

#include "util/string_utils.hpp"

using namespace vfrb::object;
using namespace vfrb::str_util;

namespace vfrb::feed::parser
{
Atmosphere AtmosphereParser::unpack(Str&& sentence, u32 priority) const
{
    try
    {
        if (MatchChecksum({sentence.c_str(), sentence.length()}) && sentence.find("MDA") != Str::npos)
        {
            usize tmpB;
            if ((tmpB = sentence.find('B')) != Str::npos)
            {
                --tmpB;
            }
            else
            {
                throw error::UnpackError();
            }
            usize tmpS;
            if ((tmpS = std::string_view(sentence.c_str(), tmpB).find_last_of(',')) != Str::npos)
            {
                ++tmpS;
            }
            else
            {
                throw error::UnpackError();
            }
            if (auto [v, ec] = Convert<f64>(sentence.c_str() + tmpS, sentence.c_str() + tmpB); ec == Errc::OK)
            {
                Atmosphere atmos{priority, v * 1000.0};
                *atmos = std::move(sentence);
                return atmos;
            }
        }
    }
    catch ([[maybe_unused]] str_util::error::ConversionError const&)
    {}
    throw error::UnpackError();
}
}  // namespace vfrb::feed::parser
