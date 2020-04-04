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

#include "concurrent/CObjectContainer.hpp"
#include "object/CAircraft.hpp"
#include "processor/CAircraftProcessor.hpp"

#include "IData.hpp"
#include "types.hpp"

namespace vfrb::data
{
/**
 * @brief Store aircrafts.
 */
class CAircraftData : public IData
{
    CTCONST NO_FLARM_THRESHOLD = object::CObject::OUTDATED;  ///< Times until FLARM status is removed
    CTCONST DELETE_THRESHOLD   = 120;                        ///< Times until aircraft gets deleted
    CTCONST NMEA_SIZE          = processor::CAircraftProcessor::NMEA_SIZE;

    concurrent::CObjectContainer<object::CAircraft, NMEA_SIZE>
                                  m_container;  ///< Internal container for aircrafts
    processor::CAircraftProcessor m_processor;  ///< Processor for aircrafts

public:
    explicit CAircraftData(AccessFn&& fn_);
    CAircraftData(AccessFn&& fn_, s32 maxDist_);  ///< @param maxDist The max distance filter

    /**
     * @brief Insert or update an Aircraft.
     * @param aircraft The update
     * @return true on success, else false
     * @threadsafe
     */
    bool Update(object::CObject&& aircraft_) override;

    void Environment(object::SLocation const& loc_, f64 press_);

    /**
     * @brief Process all aircrafts.
     * @param position The refered position
     * @param atmPress The atmospheric pressure
     * @threadsafe
     */
    void Access() override;
};
}  // namespace vfrb::data
