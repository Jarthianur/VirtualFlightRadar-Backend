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

#include <cstddef>
#include <stdexcept>

#include "util/math.hpp"

namespace feed
{
namespace parser
{
AtmosphereParser::AtmosphereParser() : Parser<object::Atmosphere>() {}

object::Atmosphere AtmosphereParser::unpack(const std::string& sentence, std::uint32_t priority) const
{
    try
    {
        if ((std::stoi(sentence.substr(sentence.rfind('*') + 1, 2), nullptr, 16) ==
             math::checksum(sentence.c_str(), sentence.length())) &&
            (sentence.find("MDA") != std::string::npos))
        {
            std::size_t tmpB   = sentence.find('B') - 1;
            std::size_t tmpS   = sentence.substr(0, tmpB).find_last_of(',') + 1;
            std::size_t subLen = tmpB - tmpS;
            std::size_t numIdx;
            double      tmpPress = std::stod(sentence.substr(tmpS, subLen), &numIdx) * 1000.0;
            if (numIdx == subLen)
            {
                object::Atmosphere atmos{priority, tmpPress};
                *atmos = sentence;
                return atmos;
            }
        }
    }
    catch (const std::exception&)
    {}
    throw UnpackError();
}

}  // namespace parser
}  // namespace feed
