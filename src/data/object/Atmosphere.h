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

#include <cstdint>

#include "../../Defines.h"
#include "Object.h"

/// ICAO standard atmospheric pressure at MSL
#define ICAO_STD_A 1013.25

/// @namespace data
namespace data
{
/// @namespace object
namespace object
{
struct Climate;

/**
 *@class Atmosphere
 * @brief Represent atmospheric information.
 * @extends Object
 */
class Atmosphere : public Object
{
public:
    /**
     * @fn Atmosphere
     * @brief Constructor
     * @param vPriority The initial priority (default: 0)
     */
    explicit Atmosphere(std::uint32_t vPriority = 0);

    /**
     * @fn Atmosphere
     * @brief Constructor
     * @param vPressure The pressure value
     */
    explicit Atmosphere(double vPressure);

    /**
     * @fn ~Atmosphere
     * @brief Destructor
     */
    virtual ~Atmosphere() noexcept;

    /**
     * @fn assign
     * @brief Assign other Atmosphere's values to this.
     * @param crOther The other Atmosphere
     */
    void assign(const Object& crOther) override;

    /**
     * Define and declare getters and setters.
     */
    GETTER_V(double, mPressure, Pressure)

private:
    friend struct Climate;

    /// @var mPressure
    /// The atmospheric pressure
    double mPressure = ICAO_STD_A;
};

}  // namespace object
}  // namespace data
