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

#include <stdexcept>

#include "util/utility.hpp"

using namespace vfrb::object;

namespace vfrb::feed::parser
{
Atmosphere AtmosphereParser::unpack(str&& sentence, u32 priority) const
{
    try
    {
        if (util::matchChecksum({sentence.c_str(), sentence.length()}) && sentence.find("MDA") != str::npos)
        {
            usize tmpB   = sentence.find('B') - 1;
            usize tmpS   = sentence.substr(0, tmpB).find_last_of(',') + 1;
            usize subLen = tmpB - tmpS;
            usize numIdx;
            f64   tmpPress = std::stod(sentence.substr(tmpS, subLen), &numIdx) * 1000.0;
            if (numIdx == subLen)
            {
                Atmosphere atmos{priority, tmpPress};
                *atmos = std::move(sentence);
                return atmos;
            }
        }
    }
    catch ([[maybe_unused]] std::logic_error const&)
    {}
    throw error::UnpackError();
}
}  // namespace vfrb::feed::parser
