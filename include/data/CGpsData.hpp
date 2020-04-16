/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <tuple>

#include "concurrent/Mutex.hpp"
#include "error/IError.hpp"
#include "object/CGpsPosition.hpp"
#include "processor/CGpsProcessor.hpp"

#include "IData.hpp"
#include "types.hpp"

namespace vfrb::data
{
/**
 * @brief Store GPS information.
 */
class CGpsData : public IData
{
    CTCONST GPS_NR_SATS_GOOD      = 7;    ///< Good number of satellites
    CTCONST GPS_FIX_GOOD          = 1;    ///< Good fix quality
    CTCONST GPS_HOR_DILUTION_GOOD = 2.0;  ///< Good horizontal dilution
    CTCONST NMEA_SIZE             = processor::CGpsProcessor::NMEA_SIZE;

    concurrent::Mutex mutable m_mutex;
    std::tuple<object::CGpsPosition, CStaticString<NMEA_SIZE>>
                             GUARDED_BY(m_mutex) m_position;   ///< The position
    processor::CGpsProcessor GUARDED_BY(m_mutex) m_processor;  ///< Processor for GPS information
    bool GUARDED_BY(m_mutex) m_positionLocked = false;         ///< Locking state of the current position
    bool GUARDED_BY(m_mutex) m_groundMode     = false;         ///< Ground mode state

    /**
     * @brief Check whether the position is good enough.
     * @return true if yes, else false
     */
    bool isPositionGood() const REQUIRES(m_mutex);

public:
    /**
     * @param crPosition The initial info
     */
    CGpsData(AccessFn&& fn_, object::CGpsPosition const& pos_, bool gnd_);

    /**
     * @brief Update the position.
     * @param position The new position
     * @return true on success, else false
     * @throw PositionAlreadyLocked if the position was locked before
     * @throw ReceivedGoodPosition if the position was good and ground mode is enabled, hence locked
     * @threadsafe
     */
    bool Update(object::CObject&& pos_) override REQUIRES(!m_mutex);

    void Access() override REQUIRES(!m_mutex);

    /**
     * @brief Get the position.
     * @return the position
     * @threadsafe
     */
    decltype(std::get<0>(m_position).Location()) Location() const REQUIRES(!m_mutex);
};

namespace error
{
class IGpsDataException : public vfrb::error::IError
{};

/**
 * @brief Exception to signal that position was already locked.
 */
class CPositionAlreadyLocked : public IGpsDataException
{
public:
    str Message() const noexcept override;
};

/**
 * @brief Exception to signal that the position was good.
 */
class CReceivedGoodPosition : public IGpsDataException
{
public:
    str Message() const noexcept override;
};
}  // namespace error
}  // namespace vfrb::data
