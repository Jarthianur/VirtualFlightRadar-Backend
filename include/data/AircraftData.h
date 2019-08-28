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

#include "concurrency/ConcurrentContainer.hpp"
#include "object/Aircraft.h"
#include "processor/AircraftProcessor.h"
#include "util/types.h"

#include "Data.hpp"

namespace vfrb::data
{
/**
 * @brief Store aircrafts.
 */
class AircraftData : public Data
{
    inline static constexpr auto NO_FLARM_THRESHOLD =
        object::Object::OUTDATED;                         ///< Times until FLARM status is removed
    inline static constexpr auto DELETE_THRESHOLD = 120;  ///< Times until aircraft gets deleted

    concurrent::ConcurrentContainer<object::Aircraft> m_container;  ///< Internal container for aircrafts
    processor::AircraftProcessor                      m_processor;  ///< Processor for aircrafts

public:
    explicit AircraftData(AccessFn&& fn);
    AircraftData(AccessFn&& fn, s32 maxDist);  ///< @param maxDist The max distance filter
    ~AircraftData() noexcept override = default;

    /**
     * @brief Insert or update an Aircraft.
     * @param aircraft The update
     * @return true on success, else false
     * @threadsafe
     */
    bool update(object::Object&& aircraft) override;

    void environment(object::Location const& position, f64 atmPress);

    /**
     * @brief Process all aircrafts.
     * @param position The refered position
     * @param atmPress The atmospheric pressure
     * @threadsafe
     */
    void access() override;
};
}  // namespace vfrb::data
