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

#include "Object.h"

namespace object
{
struct Climate;

/**
 * @brief Extend Object to atmospheric information.
 */
class Atmosphere : public Object
{
    static constexpr auto ICAO_STD     = 1013.25;  ///< ICAO standard atmospheric pressure at MSL
    static constexpr auto MAX_PRESSURE = 2000.0;
    static constexpr auto MIN_PRESSURE = 0.0;

    double      m_pressure = ICAO_STD;  ///< The atmospheric pressure
    std::string m_nmea;

    /**
     * @brief Extend Object::assign.
     */
    void assign(Object&& other) override;

public:
    Atmosphere();
    explicit Atmosphere(std::uint32_t priority);  ///< @param priority The initial priority

    /**
     * @brief Constructor
     * @param pressure The initial pressure
     * @param priority The initial priority
     */
    Atmosphere(std::uint32_t priority, double pressure);
    ~Atmosphere() noexcept override = default;

    std::string& operator*();

    util::CStringPack nmea() const override;
    auto              pressure() const -> decltype(m_pressure);
};

}  // namespace object
