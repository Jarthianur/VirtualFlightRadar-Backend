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

#include "concurrent/CObjectContainer.hpp"
#include "object/CAircraft.hpp"
#include "processor/CAircraftProcessor.hpp"

#include "IData.hpp"
#include "Types.hpp"

namespace vfrb::data
{
class CAircraftData : public IData
{
public:
    CTCONST NO_FLARM_THRESHOLD = object::CObject::OUTDATED;
    CTCONST DELETE_THRESHOLD   = 120;

    CAircraftData();
    explicit CAircraftData(i32 maxDist_);

    auto
    Update(object::CObject&& aircraft_) -> bool override;

    void
    Environment(object::SLocation const& loc_, f64 press_);

    void
    CollectInto(str_util::StringInserter si_) override;

    auto
    Size() const -> usize override;

private:
    concurrent::CObjectContainer<object::CAircraft> m_container;
    processor::CAircraftProcessor                   m_processor;

// allow access to underlying container when testing
#ifdef VFRB_TEST_MODE
public:
    auto
    Container() -> decltype(m_container)& {
        return m_container;
    }
    auto
    Get(char const* id_) -> object::CAircraft const& {
        auto it = m_container.Begin();
        while (it != m_container.End()) {
            if (it->Value.Id() == CStaticString<8>(id_)) {
                return it->Value;
            }
            ++it;
        }
        throw std::out_of_range(id_);
    }
#endif
};
}  // namespace vfrb::data
