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

#include "util/utility.hpp"

#include "Object.h"

namespace data
{
class AtmosphereData;
}
namespace feed
{
namespace parser
{
class AtmosphereParser;
}
}  // namespace feed

namespace object
{
struct Climate;

/**
 * @brief Extend Object to atmospheric information.
 */
class Atmosphere : public Object
{
    friend class data::AtmosphereData;
    friend class feed::parser::AtmosphereParser;

    //< begin constants >//
    static constexpr const double      ICAO_STD  = 1013.25;  ///< ICAO standard atmospheric pressure at MSL
    static constexpr const std::size_t NMEA_SIZE = 4096;
    //< end constants >//

    //< begin members >//
    double                   m_pressure = ICAO_STD;  ///< The atmospheric pressure
    util::CString<NMEA_SIZE> m_nmea;
    //< end members >//

    //< begin methods >//
    /**
     * @brief Extend Object::assign.
     */
    void assign(Object&& other) override;
    //< end methods >//

public:
    Atmosphere();
    explicit Atmosphere(std::uint32_t priority);  ///< @param priority The initial priority

    /**
     * @brief Constructor
     * @param pressure The initial pressure
     * @param priority The initial priority
     */
    Atmosphere(double pressure, std::uint32_t priority);
    ~Atmosphere() noexcept override = default;

    //< begin interfaces >//
    util::CStringPack getNMEA() const override;
    //< end interfaces >//
};

}  // namespace object
