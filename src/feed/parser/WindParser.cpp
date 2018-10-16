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

#include "WindParser.h"

#include <stdexcept>

#include "../../util/math.hpp"

namespace feed
{
namespace parser
{
WindParser::WindParser() : Parser<object::Wind>() {}

WindParser::~WindParser() noexcept {}

bool WindParser::unpack(const std::string& sentence, object::Wind& wind) noexcept
{
    try
    {
        if ((std::stoi(sentence.substr(sentence.rfind('*') + 1, 2), nullptr, 16) ==
             math::checksum(sentence.c_str(), sentence.length())) &&
            (sentence.find("MWV") != std::string::npos))
        {
            wind.set_serialized(std::string(sentence));
            return true;
        }
    }
    catch (const std::logic_error&)
    {}
    return false;
}

}  // namespace parser
}  // namespace feed
