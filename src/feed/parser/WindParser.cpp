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
WindParser::WindParser() : Parser<Wind>() {}

Wind WindParser::unpack(str&& sentence, u32 priority) const
{
    if (str_util::matchChecksum({sentence.c_str(), sentence.length()}) && sentence.find("MWV") != str::npos)
    {
        Wind wind{priority};
        *wind = std::move(sentence);
        return wind;
    }
    throw error::UnpackError();
}
}  // namespace vfrb::feed::parser
