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
#include "object/CWind.hpp"

#include "IData.hpp"
#include "types.hpp"

namespace vfrb::data
{
/**
 * @brief Store wind information.
 */
class CWindData : public IData
{
    concurrent::Mutex mutable m_mutex;
    object::CWind GUARDED_BY(m_mutex) m_wind;  ///< The Wind information

public:
    explicit CWindData(AccessFn&& fn_);
    CWindData(AccessFn&& fn_, object::CWind&& wind_);  ///< @param wind The initial wind information

    /**
     * @brief Update the wind information.
     * @param wind The new wind information.
     * @return true on success, else false
     * @threadsafe
     */
    bool Update(object::CObject&& wind_) override REQUIRES(!m_mutex);

    void Access() override REQUIRES(!m_mutex);
};
}  // namespace vfrb::data
