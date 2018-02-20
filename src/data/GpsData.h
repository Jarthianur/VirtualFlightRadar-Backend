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

#include "../util/GpsModule.h"
#include "../util/Position.h"
#include "../util/Wrapper.hpp"

namespace feed
{
class GpsFeed;
}

/// @namespace data
namespace data
{
/**
 * @class GpsData
 * @brief Manage GPS information.
 */
class GpsData
{
public:
    /**
     * @fn GpsData
     * @brief Constructor
     * @param vPosition The initial info
     */
    explicit GpsData(struct util::ExtGpsPosition vPosition);

    /**
     * @fn ~GpsData
     * @brief Destructor
     */
    virtual ~GpsData() noexcept;

    /**
     * @fn getGpsStr
     * @brief Get a full NMEA GPS report.
     * @note A full report contains GPGGA and GPRMC and includes trailing <cr><lf>.
     * @return the NMEA string
     * @threadsafe
     */
    std::string getGpsStr();

    /**
     * @fn getBasePos
     * @brief Get the base GPS position.
     * @return the position
     * @threadsafe
     */
    struct util::GpsPosition getBasePos();

protected:
    friend class feed::GpsFeed;

    /// @var mMutex
    /// Used for RW on this data
    boost::mutex mMutex;

    /**
     * @fn lockPosition
     * @brief Set the current position to locked
     */
    void lockPosition();

    /**
     * @fn update
     * @brief Try to update the base position.
     * @param crPosition The new position
     * @param vPriority  The attempts priority
     * @param rAttempts  The update attempts
     */
    void update(const struct util::ExtGpsPosition& crPosition, std::uint32_t vPriority,
                std::uint64_t& rAttempts);

private:
    /// @var mBasePos
    /// Wrapper holding the base position
    struct util::Wrapper<struct util::ExtGpsPosition> mBasePos;

    /// @var mGpsModule
    /// GpsModule providing functionality to build GPS sentences
    util::GpsModule mGpsModule;

    /// @var mPosLocked
    /// Locking state of the current position
    bool mPosLocked = false;
};

}  // namespace data
