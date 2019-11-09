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

#include "concurrent/Mutex.hpp"
#include "object/Atmosphere.h"

#include "Data.hpp"
#include "types.h"

namespace vfrb::data
{
/**
 * @brief Store atmospheric information.
 */
class CAtmosphereData : public IData
{
    concurrent::Mutex mutable m_mutex;
    object::CAtmosphere GUARDED_BY(m_mutex) m_atmosphere;  ///< Atmospheric information

public:
    explicit CAtmosphereData(AccessFn&& fn_);
    CAtmosphereData(AccessFn&&                 fn_,
                    object::CAtmosphere const& atmosphere_);  ///< @param atmosphere The initial atm info
    ~CAtmosphereData() noexcept override = default;

    /**
     * @brief Update he athmosphere data.
     * @param atmosphere The new atm info
     * @return true on success, else false
     * @threadsafe
     */
    bool Update(object::CObject&& atmosphere_) override REQUIRES(!m_mutex);

    void Access() override REQUIRES(!m_mutex);

    /**
     * @brief Get the atmospheric pressure.
     * @return the pressure
     * @threadsafe
     */
    decltype(m_atmosphere.Pressure()) AtmPressure() const REQUIRES(!m_mutex);
};
}  // namespace vfrb::data
