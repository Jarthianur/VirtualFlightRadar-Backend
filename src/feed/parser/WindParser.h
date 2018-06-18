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

#pragma once

#include <string>

#include "../../Defines.h"
#include "../../object/Wind.h"

#include "Parser.hpp"

namespace feed
{
namespace parser
{
class WindParser : public Parser<object::Wind>
{
public:
    DEFAULT_CTOR_DTOR(WindParser)
    /**
     * @fn unpack
     * @brief Unpack into Climate.
     * @see Parser#unpack
     */
    bool unpack(const std::string& crStr, object::Wind& rWind) noexcept override;

private:
    /**
     * @fn parseClimate
     * @brief Parse a sentence and unpack into Climate.
     * @param crStr    The string to parse
     * @param rClimate The target object
     * @return true on success, else false
     * @throw std::out_of_range, std::invalid_argument from invoked functions
     */
    bool parseWind(const std::string& crStr, object::Wind& rWind);
};
}  // namespace parser
}  // namespace feed
