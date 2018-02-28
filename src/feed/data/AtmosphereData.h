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

#include "Data.h"

#include "object/Sensor.h"

namespace feed
{
/// @namespace data
namespace data
{
/**
 * @class AtmosphereData
 * @brief Manage sensor information.
 */
class AtmosphereData : public Data
{
public:
    AtmosphereData();
    /**
     * @fn AtmosphereData
     * @brief Constructor
     * @param vAtmos The initial info
     */
    explicit AtmosphereData(const object::Atmosphere& crAtmos);

    /**
     * @fn ~AtmosphereData
     * @brief Destructor
     */
    virtual ~AtmosphereData() noexcept;

    /**
     * @fn getMdaStr
     * @brief Get the MDA sentence.
     * @return the MDA sentence
     * @threadsafe
     */
    std::string getSerialized() override;

    /**
     * @fn update
     * @brief Try to update the sensor information.
     * @param crAtmos   The new data.
     * @param vPriority The priority
     * @param rAttempts The update attempts
     */
    bool update(const object::Object& crAtmos, std::size_t vSlot) override;

    /**
     * @fn getPress
     * @brief Get pressure.
     * @return the pressure
     * @threadsafe
     */
    double getAtmPress();

private:
    /// @var mAtmosphere
    /// Holding atmospheric pressure
    object::Atmosphere mAtmosphere;
};
}
}  // namespace data
