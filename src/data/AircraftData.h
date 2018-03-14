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
#include <utility>
#include <vector>

#include "object/Aircraft.h"
#include "processor/AircraftProcessor.h"
#include "Data.h"

/// @def AC_OUTDATED
/// Times until aircraft is outdated
#define AC_OUTDATED 4

///  @def AC_DELETE_THRESHOLD
/// Times until aircraft gets deleted
#define AC_DELETE_THRESHOLD 120

/// @def AC_NO_FLARM_THRESHOLD
/// Times until FLARM status is removed
#define AC_NO_FLARM_THRESHOLD AC_OUTDATED

/// @namespace data
namespace data
{
/**
 * @class AircraftData
 * @brief Store Aircrafts and trigger processor.
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
    explicit AircraftData(std::int32_t vMaxDist);

    /**
     * @fn ~AircraftData
     * @brief Destructor
     */
    virtual ~AircraftData() noexcept;

    std::string getSerialized() override;

    /**
     * @fn upsert
     * @brief Insert or update an Aircraft, if priority is high enough.
     * @param rUpdate
     * @param vPriority
     */
    bool update(const object::Object& crAircraft, std::size_t vSlot) override;

    std::size_t registerSlot() override;

    /**
     * @fn processAircrafts
     * @brief Process all Aircrafts and get the reports.
     * @param crBasePos The base position to relate
     * @param vAtmPress The atmospheric pressure
     * @param vMaxDist  The max distance
     * @return the string with all NMEA reports
     * @threadsafe
     */
    void processAircrafts(const object::GpsPosition& crRefPosition,
                          double vAtmPress) noexcept;

private:
    void insert(const object::Aircraft& crAircraft);

    /// @var mProcessor
    /// Providing functionality to process Aircrafts
    processor::AircraftProcessor mProcessor;

    /// @var mContainer
    /// Vector holding the Aircrafts
    std::vector<object::Aircraft> mContainer;

    /// @var mIndexMap
    /// Map IDs to vector indices to make find efficient
    std::unordered_map<std::string, std::pair<std::size_t, std::vector<std::uint32_t>>>
        mIndexMap;

    std::size_t nrOfRegisteredFeeds = 0;
};

}  // namespace data
