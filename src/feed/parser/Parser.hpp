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

namespace feed
{
namespace parser
{
/**
 * @class Parser
 * @brief Interface for parsers.
 * @tparam T The corresponding data type
 */
template<typename T>
class Parser
{
public:
    /**
     * @fn Parser
     * @brief Constructor
     */
    inline Parser()
    {}

    /**
     * @fn ~Parser
     * @brief Destructor
     */
    inline virtual ~Parser() noexcept
    {}

    /**
     * @fn unpack
     * @brief Unpack a given string into the templated object.
     * @note To be impemented.
     * @param cr_msg The msg to unpack
     * @tparam T     The object where to unpack into
     * @return true on success, else false
     */
    virtual bool unpack(const std::string& crStr, T&) noexcept = 0;
};

}  // namespace parser
}
