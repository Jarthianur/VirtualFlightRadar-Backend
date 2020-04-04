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

#include "data/CAircraftData.hpp"

#include "error/IError.hpp"

using namespace vfrb::object;

namespace vfrb::data
{
CAircraftData::CAircraftData(AccessFn&& fn_) : CAircraftData(std::move(fn_), 0) {}

CAircraftData::CAircraftData(AccessFn&& fn_, s32 maxDist_) : IData(std::move(fn_)), m_processor(maxDist_) {}

bool CAircraftData::Update(CObject&& aircraft_) {
    auto&& aircraft = static_cast<CAircraft&&>(aircraft_);
    auto   result   = m_container.Insert(std::hash<StringView>()(*aircraft.Id()), std::move(aircraft));
    if (!result.second) {
        result.first->Value.TryUpdate(std::move(aircraft));
    }
    return true;
}

void CAircraftData::Environment(SLocation const& loc_, f64 press_) {
    m_processor.ReferTo(loc_, press_);
}

void CAircraftData::Access() {
    auto iter = m_container.Begin();
    while (iter != m_container.End()) {
        ++(iter->Value);
        try {
            if (iter->Value.UpdateAge() == NO_FLARM_THRESHOLD) {
                iter->Value.TargetType(CAircraft::ETargetType::TRANSPONDER);
            }
            if (iter->Value.UpdateAge() >= DELETE_THRESHOLD) {
                auto key = iter.Key();
                ++iter;
                m_container.Erase(key);
            } else {
                m_processor.Process(iter->Value, iter->Nmea);
                m_accessFn({iter->Value, iter->Nmea});
                ++iter;
            }
        } catch ([[maybe_unused]] vfrb::error::IError const&) {
        }
    }
}
}  // namespace vfrb::data
