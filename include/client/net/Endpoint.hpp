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

namespace client
{
namespace net
{
/**
 * @brief A remote endpoint
 */
struct Endpoint
{
    //< begin members >//
    const std::string host;  ///< Hostname
    const std::string port;  ///< Port number
    //< end members >//

    //< begin operators >//
    /**
     * @brief Equality comparison by value
     * @param other The other endpoint
     * @return true if both are equal, else false
     */
    bool operator==(const Endpoint& other) const
    {
        return host == other.host && port == other.port;
    }
    //< end operators >//
};
}  // namespace net
}  // namespace client
