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

#include "Data.hpp"

///  @def AC_DELETE_THRESHOLD
/// Times until aircraft gets deleted
#define AC_DELETE_THRESHOLD 120

/// @def AC_NO_FLARM_THRESHOLD
/// Times until FLARM status is removed
#define AC_NO_FLARM_THRESHOLD OBJ_OUTDATED

/// @namespace data
namespace data
{
/**
 * @class AircraftData
 * @brief Store Aircrafts.
 * @implements Data
 */
class AircraftData : public Data
{
public:
    AircraftData();

    /**
     * @fn AircraftData
     * @brief Constructor
     * @param vMaxDist The max distance filter
     */
    explicit AircraftData(std::int32_t maxDist);

    ~AircraftData() noexcept;

    /**
     * @fn getSerialized
     * @brief Get the reports for all processed Aircrafts
     * @return the report
     * @threadsafe
     */
    void get_serialized(std::string& dest) override;

    /**
     * @fn update
     * @brief Insert or update an Aircraft.
     * @note The success depends on the registered attempts and the update priority.
     * @param rvAircraft The update
     * @param vSlot      The attempt slot
     * @return true on success, else false
     * @threadsafe
     */
    bool update(object::Object&& aircraft) override;

    /**
     * @fn processAircrafts
     * @brief Process all Aircrafts.
     * @param crRefPosition The refered position
     * @param vAtmPress The atmospheric pressure
     * @threadsafe
     */
    void processAircrafts(const object::Position& position, double atmPress) noexcept;

private:
    /**
     * @fn insert
     * @brief Insert an Aircraft in the internal container.
     * @param crAircraft The aircraft
     */
    void insert(const object::Aircraft& aircraft);

    /// @var mProcessor
    /// Providing functionality to process Aircrafts
    processor::AircraftProcessor m_processor;

    /// @var mContainer
    /// Vector holding the Aircrafts
    std::vector<object::Aircraft> m_container;

    /// @var mIndexMap
    /// Map aircraft Id's to index and attempt counters.
    std::unordered_map<std::string, std::size_t> m_index;
};

}  // namespace data
