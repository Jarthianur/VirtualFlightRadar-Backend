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
 * @class AtmosphereData
 * @brief Manage sensor information.
 */
class AtmosphereData
{
public:
    /**
     * @fn AtmosphereData
     * @brief Constructor
     * @param vAtmos The initial info
     */
    explicit AtmosphereData(struct util::Atmosphere vAtmos);

    /**
     * @fn ~AtmosphereData
     * @brief Destructor
     */
    virtual ~AtmosphereData() noexcept;

    /**
     * @fn getMdaStr
     * @brief Get the MDA sentence.
     * @return the MDA sentence
     * @threadsafe
     */
    std::string getMdaStr();

    /**
     * @fn getPress
     * @brief Get pressure.
     * @return the pressure
     * @threadsafe
     */
    double getAtmPress();

protected:
    friend class feed::SensorFeed;

    /// @var mMutex
    /// Used for RW on this data
    boost::mutex mMutex;

    /**
     * @fn update
     * @brief Try to update the sensor information.
     * @param crAtmos   The new data.
     * @param vPriority The priority
     * @param rAttempts The update attempts
     */
    void update(const struct util::Atmosphere& crAtmos, std::uint32_t vPriority,
                std::uint64_t& rAttempts);

private:
    /// @var mAtmosphere
    /// Holding atmospheric pressure
    struct util::Wrapper<util::Atmosphere> mAtmosphere;
};

}  // namespace data
