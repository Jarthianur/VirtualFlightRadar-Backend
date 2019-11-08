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
#include "object/Wind.h"

#include "Data.hpp"
#include "types.h"

namespace vfrb::data
{
/**
 * @brief Store wind information.
 */
class WindData : public Data
{
    concurrent::Mutex mutable m_mutex;

    object::Wind GUARDED_BY(m_mutex) m_wind;  ///< The Wind information

public:
    explicit WindData(AccessFn&& fn);
    WindData(AccessFn&& fn, object::CWind const& wind);  ///< @param wind The initial wind information
    ~WindData() noexcept override = default;

    /**
     * @brief Update the wind information.
     * @param wind The new wind information.
     * @return true on success, else false
     * @threadsafe
     */
    bool update(object::Object&& wind) override REQUIRES(!m_mutex);

    void access() override REQUIRES(!m_mutex);
};
}  // namespace vfrb::data
