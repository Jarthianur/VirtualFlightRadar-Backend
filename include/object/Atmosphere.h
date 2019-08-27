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

#include "util/types.h"

#include "Object.h"

namespace vfrb::object
{
struct Climate;

/**
 * @brief Extend Object to atmospheric information.
 */
class Atmosphere : public Object
{
    inline static constexpr auto ICAO_STD     = 1013.25;  ///< ICAO standard atmospheric pressure at MSL
    inline static constexpr auto MAX_PRESSURE = 2000.0;
    inline static constexpr auto MIN_PRESSURE = 0.0;

    f64 m_pressure = ICAO_STD;  ///< The atmospheric pressure
    str m_nmea;

    /**
     * @brief Extend Object::assign.
     */
    void assign(Object&& other) override;

public:
    Atmosphere();
    explicit Atmosphere(u32 priority);  ///< @param priority The initial priority

    /**
     * @brief Constructor
     * @param pressure The initial pressure
     * @param priority The initial priority
     */
    Atmosphere(u32 priority, f64 pressure);
    ~Atmosphere() noexcept override = default;

    str& operator*();

    std::string_view nmea() const override;
    auto             pressure() const -> decltype(m_pressure);
};
}  // namespace vfrb::object
