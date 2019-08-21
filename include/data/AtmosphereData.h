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

#include <mutex>

#include "object/Atmosphere.h"

#include "Data.hpp"

namespace data
{
/**
 * @brief Store atmospheric information.
 */
class AtmosphereData : public Data
{
    object::Atmosphere m_atmosphere;  ///< Atmospheric information
    mutable std::mutex m_mutex;

public:
    AtmosphereData();
    explicit AtmosphereData(
        const object::Atmosphere& atmosphere);  ///< @param atmosphere The initial atm info
    ~AtmosphereData() noexcept override = default;

    /**
     * @brief Update he athmosphere data.
     * @param atmosphere The new atm info
     * @return true on success, else false
     * @threadsafe
     */
    bool update(object::Object&& atmosphere) override;

    void access(const accessor_fn& func) override;

    /**
     * @brief Get the atmospheric pressure.
     * @return the pressure
     * @threadsafe
     */
    auto atmPressure() const -> decltype(m_atmosphere.pressure());
};
}  // namespace data
