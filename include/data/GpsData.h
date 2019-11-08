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

#include <tuple>

#include "concurrent/Mutex.hpp"
#include "error/Error.hpp"
#include "object/GpsPosition.h"
#include "processor/GpsProcessor.h"

#include "Data.hpp"
#include "types.h"

namespace vfrb::data
{
/**
 * @brief Store GPS information.
 */
class GpsData : public Data
{
    inline static constexpr auto GPS_NR_SATS_GOOD      = 7;    ///< Good number of satellites
    inline static constexpr auto GPS_FIX_GOOD          = 1;    ///< Good fix quality
    inline static constexpr auto GPS_HOR_DILUTION_GOOD = 2.0;  ///< Good horizontal dilution
    inline static constexpr auto NMEA_SIZE             = processor::GpsProcessor::NMEA_SIZE;

    concurrent::Mutex mutable m_mutex;
    std::tuple<object::GpsPosition, util::CString<NMEA_SIZE>>
                            GUARDED_BY(m_mutex) m_position;   ///< The position
    processor::GpsProcessor GUARDED_BY(m_mutex) m_processor;  ///< Processor for GPS information
    bool GUARDED_BY(m_mutex) m_positionLocked = false;        ///< Locking state of the current position
    bool GUARDED_BY(m_mutex) m_groundMode     = false;        ///< Ground mode state

    /**
     * @brief Check whether the position is good enough.
     * @return true if yes, else false
     */
    bool isPositionGood() const REQUIRES(m_mutex);

public:
    /**
     * @param crPosition The initial info
     */
    GpsData(AccessFn&& fn, object::CGpsPosition const& position, bool ground);
    ~GpsData() noexcept override = default;

    /**
     * @brief Update the position.
     * @param position The new position
     * @return true on success, else false
     * @throw PositionAlreadyLocked if the position was locked before
     * @throw ReceivedGoodPosition if the position was good and ground mode is enabled, hence locked
     * @threadsafe
     */
    bool update(object::Object&& position) override REQUIRES(!m_mutex);

    void access() override REQUIRES(!m_mutex);

    /**
     * @brief Get the position.
     * @return the position
     * @threadsafe
     */
    decltype(std::get<0>(m_position).location()) location() const REQUIRES(!m_mutex);
};

namespace error
{
class GpsDataException : public vfrb::error::IError
{
protected:
    GpsDataException()                            = default;
    virtual ~GpsDataException() noexcept override = default;
};

/**
 * @brief Exception to signal that position was already locked.
 */
class PositionAlreadyLocked : public GpsDataException
{
public:
    PositionAlreadyLocked()                    = default;
    ~PositionAlreadyLocked() noexcept override = default;
    char const* what() const noexcept override;
};

/**
 * @brief Exception to signal that the position was good.
 */
class ReceivedGoodPosition : public GpsDataException
{
public:
    ReceivedGoodPosition()                    = default;
    ~ReceivedGoodPosition() noexcept override = default;
    char const* what() const noexcept override;
};
}  // namespace error
}  // namespace vfrb::data
