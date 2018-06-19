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

#include "../Defines.h"
#include "../object/Atmosphere.h"
#include "Data.h"

/// @namespace data
namespace data
{
/**
 * @class AtmosphereData
 * @brief Store atmospheric information.
 * @implements Data
 */
class AtmosphereData : public Data
{
public:
    DEFAULT_CTOR_DTOR(AtmosphereData)

    /**
     * @fn AtmosphereData
     * @brief Constructor
     * @param vAtmosphere The initial info
     */
    explicit AtmosphereData(const object::Atmosphere& crAtmosphere);

    /**
     * @fn getSerialized
     * @brief Get the MDA sentence.
     * @return the MDA sentence
     * @threadsafe
     */
    std::string getSerialized() override;

    /**
     * @fn update
     * @brief Attempt to update.
     * @param rvAtmosphere The new data.
     * @param vSlot        The attempt slot
     * @return true on success, else false
     * @threadsafe
     */
    bool update(object::Object&& rvAtmosphere) override;

    /**
     * @fn getAtmPressure
     * @brief Get the atmospheric pressure.
     * @return the pressure
     * @threadsafe
     */
    double getAtmPressure();

private:
    /// @var mAtmosphere
    /// Holding atmospheric information
    object::Atmosphere mAtmosphere;
};
}  // namespace data
