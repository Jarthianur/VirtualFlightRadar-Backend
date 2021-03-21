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

using vfrb::object::CObject;
using vfrb::object::CGpsPosition;
using vfrb::concurrent::ImmutableLock;
using vfrb::str_util::StringInserter;

namespace vfrb::data
{
CGpsData::CGpsData(CGpsPosition const& pos_, bool gnd_) : m_position(pos_), m_groundMode(gnd_) {}

void
CGpsData::CollectInto(StringInserter si_) {
    ImmutableLock lk(m_mutex);
    try {
        m_processor.Process(m_position, si_);
        ++m_position;
    } catch ([[maybe_unused]] vfrb::error::IError const&) {
    }
}

auto
CGpsData::Update(CObject&& pos_) -> bool {
    ImmutableLock lk(m_mutex);
    if (m_positionLocked) {
        throw error::CPositionAlreadyLocked();
    }
    bool updated = m_position.TryUpdate(std::move(pos_));
    if (updated) {
        if (m_groundMode && isPositionGood()) {
            m_positionLocked = true;
            throw error::CReceivedGoodPosition();
        }
    }
    return updated;
}

auto
CGpsData::Location() const -> decltype(m_position.Location()) {
    ImmutableLock lk(m_mutex);
    return m_position.Location();
}

auto
CGpsData::isPositionGood() const -> bool {
    return m_position.NrOfSatellites() >= GPS_NR_SATS_GOOD && m_position.FixQuality() >= GPS_FIX_GOOD &&
           m_position.Dilution() <= GPS_HOR_DILUTION_GOOD;
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
