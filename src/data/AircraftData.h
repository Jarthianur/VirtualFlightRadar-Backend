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
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <boost/thread/mutex.hpp>

#include "../aircraft/Aircraft.h"
#include "../aircraft/AircraftProcessor.h"
#include "../config/Parameters.h"

/// @def AC_OUTDATED
/// Times until aircraft is outdated
#define AC_OUTDATED 4

///  @def AC_DELETE_THRESHOLD
/// Times until aircraft gets deleted
#define AC_DELETE_THRESHOLD 120

/// @def AC_NO_FLARM_THRESHOLD
/// Times until FLARM status is removed
#define AC_NO_FLARM_THRESHOLD AC_OUTDATED

namespace feed
{
class AprscFeed;
class SbsFeed;
}

namespace util
{
struct GpsPosition;
}

/// @namespace data
namespace data
{
/**
 * @class AircraftData
 * @brief Store Aircrafts and trigger processing.
 */
class AircraftData
{
public:
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

    /**
     * @fn processAircrafts
     * @brief Process all Aircrafts and get the reports.
     * @param crBasePos The base position to relate
     * @param vAtmPress The atmospheric pressure
     * @param vMaxDist  The max distance
     * @return the string with all NMEA reports
     * @threadsafe
     */
    std::string processAircrafts(const struct util::GpsPosition& crBasePos,
                                 double vAtmPress) noexcept;

protected:
    friend class feed::AprscFeed;
    friend class feed::SbsFeed;

    /// @var mMutex
    /// Used for RW on the container.
    boost::mutex mMutex;

    /**
     * @fn upsert
     * @brief Insert or update an Aircraft, if priority is high enough.
     * @param rUpdate
     * @param vPriority
     */
    void update(const aircraft::Aircraft& crUpdate, std::uint32_t vPriority);

private:
    /**
     * @fn find
     * @brief Find an Aircraft by Id efficiently in the container using an index map.
     * @param crId The Id to search
     * @return an iterator to the Aircraft if found, else vector::end
     */
    std::vector<aircraft::Aircraft>::iterator find(const std::string& crId);

    /// @var mProcessor
    /// Providing functionality to process Aircrafts
    aircraft::AircraftProcessor mProcessor;

    /// @var mContainer
    /// Vector holding the Aircrafts
    std::vector<aircraft::Aircraft> mContainer;

    /// @var mIndexMap
    /// Map IDs to vector indices to make find efficient
    std::unordered_map<std::string, std::size_t> mIndexMap;
};

}  // namespace data
