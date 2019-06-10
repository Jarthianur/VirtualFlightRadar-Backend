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

#include <exception>
#include <mutex>

#include "object/GpsPosition.h"
#include "processor/GpsProcessor.h"

#include "Data.hpp"

namespace data
{
/**
 * @brief Store GPS information.
 */
class GpsData : public Data
{
    //< begin constants >//
    static constexpr auto GPS_NR_SATS_GOOD      = 7;    ///< Good number of satellites
    static constexpr auto GPS_FIX_GOOD          = 1;    ///< Good fix quality
    static constexpr auto GPS_HOR_DILUTION_GOOD = 2.0;  ///< Good horizontal dilution
    //< end constants >//

    //< begin members >//
    object::GpsPosition     m_position;                ///< The position
    processor::GpsProcessor m_processor;               ///< Processor for GPS information
    bool                    m_positionLocked = false;  ///< Locking state of the current position
    bool                    m_groundMode     = false;  ///< Ground mode state
    mutable std::mutex      m_mutex;
    //< end members >//

    //< begin methods >//
    /**
     * @brief Check whether the position is good enough.
     * @return true if yes, else false
     */
    bool isPositionGood() const;
    //< end methods >//

public:
    /**
     * @param crPosition The initial info
     */
    GpsData(const object::GpsPosition& position, bool ground);
    ~GpsData() noexcept override = default;

    //< begin interfaces >//
    /**
     * @brief Update the position.
     * @param position The new position
     * @return true on success, else false
     * @throw PositionAlreadyLocked if the position was locked before
     * @throw ReceivedGoodPosition if the position was good and ground mode is enabled, hence locked
     * @threadsafe
     */
    bool update(object::Object&& position) override;

    void access(const accessor_fn& func) override;

    /**
     * @brief Get the position.
     * @return the position
     * @threadsafe
     */
    auto getLocation() const -> decltype(m_position.getLocation());
    //< end interfaces >//
};

class GpsDataException : public std::exception
{
protected:
    GpsDataException() : std::exception() {}
    virtual ~GpsDataException() noexcept = default;
};

/**
 * @brief Exception to signal that position was already locked.
 */
class PositionAlreadyLocked : public GpsDataException
{
public:
    PositionAlreadyLocked();
    ~PositionAlreadyLocked() noexcept override = default;
    const char* what() const noexcept override;
};

/**
 * @brief Exception to signal that the position was good.
 */
class ReceivedGoodPosition : public GpsDataException
{
public:
    ReceivedGoodPosition();
    ~ReceivedGoodPosition() noexcept override = default;
    const char* what() const noexcept override;
};

}  // namespace data
