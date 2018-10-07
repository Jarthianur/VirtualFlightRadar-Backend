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

#include "../util/defines.h"
#include "Object.h"

/// @def ICAO_STD_A
/// ICAO standard atmospheric pressure at MSL
#define ICAO_STD_A 1013.25

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
    Atmosphere();

    /**
     * @fn Atmosphere
     * @brief Constructor
     * @param vPriority The initial priority
     */
    explicit Atmosphere(std::uint32_t priority);

    /**
     * @fn Atmosphere
     * @brief Constructor
     * @param vPressure The initial pressure
     * @param vPriority The initial priority
     */
    Atmosphere(double pressure, std::uint32_t priority);

    ~Atmosphere() noexcept;

private:
    /**
     * @see Object#assign
     */
    void assign(Object&& other) override;

    /// @var mPressure
    /// The atmospheric pressure
    double m_pressure = ICAO_STD_A;

public:
    /**
     * Define and declare getters and setters.
     */
    GETSET_V(pressure)
};

}  // namespace object
