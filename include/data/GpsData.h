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

#include "object/GpsPosition.h"
#include "processor/GpsProcessor.h"
#include "util/types.h"

#include "Data.hpp"

namespace data
{
/**
 * @brief Store GPS information.
 */
class GpsData : public Data
{
    inline static constexpr auto GPS_NR_SATS_GOOD      = 7;    ///< Good number of satellites
    inline static constexpr auto GPS_FIX_GOOD          = 1;    ///< Good fix quality
    inline static constexpr auto GPS_HOR_DILUTION_GOOD = 2.0;  ///< Good horizontal dilution

    object::GpsPosition     m_position;                ///< The position
    processor::GpsProcessor m_processor;               ///< Processor for GPS information
    bool                    m_positionLocked = false;  ///< Locking state of the current position
    bool                    m_groundMode     = false;  ///< Ground mode state
    std::mutex mutable m_mutex;

    /**
     * @brief Check whether the position is good enough.
     * @return true if yes, else false
     */
    bool isPositionGood() const;

public:
    /**
     * @param crPosition The initial info
     */
    GpsData(AccessFn&& fn, object::GpsPosition const& position, bool ground);
    ~GpsData() noexcept override = default;

    /**
     * @brief Update the position.
     * @param position The new position
     * @return true on success, else false
     * @throw PositionAlreadyLocked if the position was locked before
     * @throw ReceivedGoodPosition if the position was good and ground mode is enabled, hence locked
     * @threadsafe
     */
    bool update(object::Object&& position) override;

    void access() override;

    /**
     * @brief Get the position.
     * @return the position
     * @threadsafe
     */
    auto location() const -> decltype(m_position.location());
};

class GpsDataException : public std::exception
{
protected:
    GpsDataException();
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
    char const* what() const noexcept override;
};

/**
 * @brief Exception to signal that the position was good.
 */
class ReceivedGoodPosition : public GpsDataException
{
public:
    ReceivedGoodPosition();
    ~ReceivedGoodPosition() noexcept override = default;
    char const* what() const noexcept override;
};
}  // namespace data
