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
/**
 * @brief Store aircrafts.
 */
class CAircraftData : public IData
{
public:
    CTCONST NO_FLARM_THRESHOLD = object::CObject::OUTDATED;  ///< Times until FLARM status is removed
    CTCONST DELETE_THRESHOLD   = 120;                        ///< Times until aircraft gets deleted
    CTCONST NMEA_SIZE          = processor::CAircraftProcessor::NMEA_SIZE;

    explicit CAircraftData(AccessFn&& fn_);
    CAircraftData(AccessFn&& fn_, s32 maxDist_);  ///< @param maxDist The max distance filter

    /**
     * @brief Insert or update an Aircraft.
     * @param aircraft The update
     * @return true on success, else false
     * @threadsafe
     */
    auto
    Update(object::CObject&& aircraft_) -> bool override;

    void
    Environment(object::SLocation const& loc_, f64 press_);

    /**
     * @brief Process all aircrafts.
     * @param position The refered position
     * @param atmPress The atmospheric pressure
     * @threadsafe
     */
    void
    Access() override;

    auto
    Size() const -> usize override;

private:
    concurrent::CObjectContainer<object::CAircraft, NMEA_SIZE>
                                  m_container;  ///< Internal container for aircrafts
    processor::CAircraftProcessor m_processor;  ///< Processor for aircrafts
};
}  // namespace vfrb::data
