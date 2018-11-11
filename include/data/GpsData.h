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
#include <string>

#include "object/GpsPosition.h"
#include "processor/GpsProcessor.h"
#include "util/defines.h"

#include "Data.hpp"

namespace data
{
/**
 * @brief Store GPS information.
 */
class GpsData : public Data
{
public:
    DEFAULT_DTOR(GpsData)

    GpsData();

    /**
     * @fn GpsData
     * @brief Constructor
     * @param crPosition The initial info
     */
    GpsData(const object::GpsPosition& position, bool ground);

    /**
     * @brief Get NMEA GPS report.
     * @param dest The destination string to append data
     * @threadsafe
     */
    void get_serialized(std::string& dest) override;

    /**
     * @brief Get the position.
     * @return the position
     * @threadsafe
     */
    object::Position get_position();

    /**
     * @brief Update the position.
     * @param position The new position
     * @return true on success and ground-mode enabled and position was good, else false
     * @throw PositionLocked if the position was locked before
     * @threadsafe
     */
    bool update(object::Object&& position) override;

private:
    /**
     * @brief Check whether the position is good enough.
     * @return true if yes, else false
     */
    bool isPositionGood();

    /// The position
    object::GpsPosition m_position;

    /// Processor for GPS information
    processor::GpsProcessor m_processor;

    /// Locking state of the current position
    bool m_positionLocked = false;

    /// Ground mode state
    bool m_groundMode = false;
};

/**
 * @brief Exception to signal that position was already locked.
 */
class PositionLocked : public std::exception
{
public:
    DEFAULT_DTOR(PositionLocked)

    PositionLocked();

    const char* what() const noexcept override;
};

}  // namespace data
