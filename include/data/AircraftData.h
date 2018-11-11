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

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "object/Aircraft.h"
#include "processor/AircraftProcessor.h"
#include "util/defines.h"

#include "Data.hpp"

/// Times until aircraft gets deleted
#define AC_DELETE_THRESHOLD 120

/// Times until FLARM status is removed
#define AC_NO_FLARM_THRESHOLD OBJ_OUTDATED

namespace data
{
/**
 * @brief Store aircrafts.
 */
class AircraftData : public Data
{
public:
    DEFAULT_DTOR(AircraftData)

    AircraftData();

    /**
     * @brief Constructor
     * @param maxDist The max distance filter
     */
    explicit AircraftData(std::int32_t maxDist);

    /**
     * @brief Get the reports for all processed aircrafts.
     * @param dest The destination string to append reports
     * @threadsafe
     */
    void get_serialized(std::string& dest) override;

    /**
     * @brief Insert or update an Aircraft.
     * @param aircraft The update
     * @return true on success, else false
     * @threadsafe
     */
    bool update(object::Object&& aircraft) override;

    /**
     * @brief Process all aircrafts.
     * @param position The refered position
     * @param atmPress The atmospheric pressure
     * @threadsafe
     */
    void processAircrafts(const object::Position& position, double atmPress) noexcept;

private:
    /**
     * @brief Insert an aircraft into the internal container.
     * @param aircraft The aircraft
     */
    void insert(object::Aircraft&& aircraft);

    /// Processor for aircrafts
    processor::AircraftProcessor m_processor;

    /// Vector holding the aircrafts
    std::vector<object::Aircraft> m_container;

    /// Map aircraft Id's to container index
    std::unordered_map<std::string, std::size_t> m_index;
};

}  // namespace data
