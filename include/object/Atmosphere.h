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

#include "util/defines.h"
#include "util/utility.hpp"

#include "Object.h"

namespace object
{
struct Climate;

/**
 * @brief Extend Object to atmospheric information.
 */
class Atmosphere : public Object
{
public:
    /// ICAO standard atmospheric pressure at MSL
    static constexpr const double      ICAO_STD  = 1013.25;
    static constexpr const std::size_t NMEA_SIZE = 4096;

    Atmosphere();
    DEFAULT_CHILD_DTOR(Atmosphere)

    /**
     * @brief Constructor
     * @param priority The initial priority
     */
    explicit Atmosphere(std::uint32_t priority);

    /**
     * @brief Constructor
     * @param pressure The initial pressure
     * @param priority The initial priority
     */
    Atmosphere(double pressure, std::uint32_t priority);

    util::CStringPack getNMEA() const override;

private:
    /**
     * @brief Extend Object::assign.
     */
    void assign(Object&& other) override;

    /// The atmospheric pressure
    double m_pressure = ICAO_STD;

    util::CString<NMEA_SIZE> m_nmea;

public:
    /**
     * Getters and setters
     */
    GETSET_V(pressure)
    GETTER_R(nmea)
};

}  // namespace object
