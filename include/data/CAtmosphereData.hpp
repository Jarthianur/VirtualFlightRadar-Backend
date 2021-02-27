/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "concurrent/Mutex.hpp"
#include "object/CAtmosphere.hpp"

#include "IData.hpp"
#include "Types.hpp"

namespace vfrb::data
{
class CAtmosphereData : public IData
{
    concurrent::Mutex mutable m_mutex;
    object::CAtmosphere GUARDED_BY(m_mutex) m_atmosphere;

public:
    explicit CAtmosphereData(object::CAtmosphere&& atm_);

    auto
    Update(object::CObject&& atm_) -> bool override REQUIRES(!m_mutex);

    void
    CollectInto(str_util::StringInserter si_) override REQUIRES(!m_mutex);

    auto
    Pressure() const -> decltype(m_atmosphere.Pressure()) REQUIRES(!m_mutex);

    auto
    Size() const -> usize override;
};
}  // namespace vfrb::data
