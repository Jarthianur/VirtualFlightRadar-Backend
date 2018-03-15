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

#include "Atmosphere.h"
#include "Wind.h"

/// @namespace data
namespace data
{
/// @namespace object
namespace object
{
/**
 *@struct Climate
 * @brief Combine Wind and Atmosphere.
 * @see Wind
 * @see Atmosphere
 */
struct Climate
{
    /// @var wind
    /// The Wind object
    Wind wind;

    /// @var atmosphere
    /// The Atmosphere object
    Atmosphere atmosphere;

    /**
     * @fn hasAtmosphere
     * @brief Is the Atmosphere object set?
     * @return true if yes, else false
     */
    inline bool hasAtmosphere()
    {
        return !atmosphere.mSerialized.empty();
    }

    /**
     * @fn hasWind
     * @brief Is the Wind object set?
     * @return true if yes, else false
     */
    inline bool hasWind()
    {
        return !wind.mSerialized.empty();
    }
};

}  // namespace object
}  // namespace data
