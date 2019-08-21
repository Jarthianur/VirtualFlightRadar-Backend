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

#include <stdexcept>

#include "util/math.hpp"

using namespace object;

namespace feed::parser
{
WindParser::WindParser() : Parser<Wind>() {}

Wind WindParser::unpack(str const& sentence, u32 priority) const
{
    try
    {
        if ((std::stoi(sentence.substr(sentence.rfind('*') + 1, 2), nullptr, 16) ==
             math::checksum(sentence.c_str(), sentence.length())) &&
            (sentence.find("MWV") != str::npos))
        {
            Wind wind{priority};
            *wind = sentence;
            return wind;
        }
    }
    catch (std::logic_error const&)
    {}
    throw UnpackError();
}
}  // namespace feed::parser
