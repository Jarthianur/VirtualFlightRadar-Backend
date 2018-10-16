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

#include <string>

#include "../object/GpsPosition.h"
#include "processor/GpsProcessor.h"

#include "Data.hpp"

/// @namespace data
namespace data
{
/**
 * @class GpsData
 * @brief Store GPS information.
 * @implements Data
 */
class GpsData : public Data
{
public:
    GpsData();

    /**
     * @fn GpsData
     * @brief Constructor
     * @param crPosition The initial info
     */
    GpsData(const object::GpsPosition& position, bool ground);

    ~GpsData() noexcept;

    /**
     * @fn getSerialized
     * @brief Get NMEA GPS report.
     * @return the NMEA string
     * @threadsafe
     */
    std::string get_serialized() override;

    /**
     * @fn getPosition
     * @brief Get the position.
     * @return the position
     * @threadsafe
     */
    object::Position get_position();

    /**
     * @fn update
     * @brief Try to update the base position.
     * @param rvPosition The new position
     * @param vSlot      The attempt slot
     * @return true on success, else false
     * @threadsafe
     */
    bool update(object::Object&& position) override;

private:
    /**
     * @fn isPositionGood
     * @brief Check whether the position is good enough.
     * @return true if yes, else false
     */
    bool isPositionGood();

    /// @var mPosition
    /// The position
    object::GpsPosition m_position;

    /// @var mProcessor
    /// Processor for GPS information
    processor::GpsProcessor m_processor;

    /// @var mPositionLocked
    /// Locking state of the current position
    bool m_positionLocked = false;

    /// @var mGroundMode
    /// Ground mode state
    bool m_groundMode = false;
};

}  // namespace data
