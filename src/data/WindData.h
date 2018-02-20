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

#include <cstdint>
#include <string>
#include <boost/thread/mutex.hpp>

#include "../util/Sensor.h"
#include "../util/Wrapper.hpp"

namespace feed
{
class SensorFeed;
}

/// @namespace data
namespace data
{
/**
 * @class WindData
 * @brief Manage sensor information.
 */
class WindData
{
public:
    /**
     * @fn WindData
     * @brief Constructor
     */
    WindData();

    /**
     * @fn ~WindData
     * @brief Destructor
     */
    virtual ~WindData() noexcept;

    /**
     * @fn getMwvStr
     * @brief Get the MWV sentence.
     * @note MWV is invalid after this operation.
     * @return the MWV sentence, if valid, else empty string
     * @threadsafe
     */
    std::string getMwvStr();

protected:
    friend class feed::SensorFeed;

    /// @var mMutex
    /// Used for RW on this data
    boost::mutex mMutex;

    /**
     * @fn update
     * @brief Try to update the sensor information.
     * @param crWind    The new wind information.
     * @param vPriority The attempts priority
     * @param rAttempts The update attempts
     */
    void update(const struct util::Wind& crWind, std::uint32_t vPriority,
                std::uint64_t& rAttempts);

private:
    /// @var mWind
    /// Holding MDA sentence
    struct util::TmpWrapper<util::Wind> mWind;
};

}  // namespace data
