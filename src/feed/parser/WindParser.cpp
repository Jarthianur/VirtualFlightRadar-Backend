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

#include <stdexcept>
#include "../../Math.hpp"

#include "WindParser.h"

namespace feed
{
namespace parser
{
WindParser::WindParser():Parser<object::Wind>()
{}

WindParser::~WindParser() noexcept{}

bool WindParser::unpack(const std::string& crStr, object::Wind& rWind) noexcept
{
    try
    {
        return (std::stoi(crStr.substr(crStr.rfind('*') + 1, 2), nullptr, 16)
                    == math::checksum(crStr.c_str(), crStr.length())
                && parseWind(crStr, rWind));
    }
    catch(const std::logic_error&)
    {
        return false;
    }
}

bool WindParser::parseWind(const std::string& crStr, object::Wind& rWind)
{
    bool valid;
    if((valid = (crStr.find("MWV") != std::string::npos)))
    {
        rWind.setSerialized(std::string(crStr));
    }
    return valid;
}

}  // namespace parser
}  // namespace feed
