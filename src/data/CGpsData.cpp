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

#include "data/CGpsData.hpp"

#include "concurrent/Mutex.hpp"
#include "object/CGpsPosition.hpp"
#include "object/CObject.hpp"

#include "CStaticString.hpp"

using vfrb::object::CObject;
using vfrb::object::CGpsPosition;
using vfrb::concurrent::LockGuard;

namespace vfrb::data
{
CGpsData::CGpsData(AccessFn&& fn_, CGpsPosition const& pos_, bool gnd_)
    : IData(std::move(fn_)), m_position(std::make_tuple(pos_, "")), m_groundMode(gnd_) {
    auto& [pos, cstr] = m_position;
    m_processor.Process(pos, &cstr);
}

void
CGpsData::Access() {
    LockGuard lk(m_mutex);
    try {
        auto& [pos, cstr] = m_position;
        m_processor.Process(pos, &cstr);
        m_accessFn({++pos, {cstr}});
    } catch ([[maybe_unused]] vfrb::error::IError const&) {
    }
}

auto
CGpsData::Update(CObject&& pos_) -> bool {
    LockGuard lk(m_mutex);
    if (m_positionLocked) {
        throw error::CPositionAlreadyLocked();
    }
    auto& [pos, cstr] = m_position;
    bool updated      = pos.TryUpdate(std::move(pos_));
    if (updated) {
        m_processor.Process(pos, &cstr);
        if (m_groundMode && isPositionGood()) {
            throw error::CReceivedGoodPosition();
        }
    }
    return updated;
}

auto
CGpsData::Location() const -> decltype(std::get<0>(m_position).Location()) {
    LockGuard lk(m_mutex);
    return std::get<0>(m_position).Location();
}

auto
CGpsData::isPositionGood() const -> bool {
    auto const& pos = std::get<0>(m_position);
    return pos.NrOfSatellites() >= GPS_NR_SATS_GOOD && pos.FixQuality() >= GPS_FIX_GOOD &&
           pos.Dilution() <= GPS_HOR_DILUTION_GOOD;
}

auto
CGpsData::Size() const -> usize {
    return 1;
}

namespace error
{
auto
CPositionAlreadyLocked::Message() const noexcept -> str {
    return "position was locked before";
}

auto
CReceivedGoodPosition::Message() const noexcept -> str {
    return "received good position";
}
}  // namespace error
}  // namespace vfrb::data
