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
#include <cstdint>
#include <string>
#include <vector>

#include "../Defines.h"
#include "../object/Position.h"
#include "processor/GpsProcessor.h"

#include "Data.h"

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
    DEFAULT_CTOR_DTOR(GpsData)

    /**
     * @fn GpsData
     * @brief Constructor
     * @param crPosition The initial info
     */
    explicit GpsData(const object::GpsPosition& crPosition, bool vGround);

    /**
     * @fn getSerialized
     * @brief Get NMEA GPS report.
     * @return the NMEA string
     * @threadsafe
     */
    std::string getSerialized() override;

    /**
     * @fn getGpsPosition
     * @brief Get the GPS position.
     * @return the position
     * @threadsafe
     */
    object::Position getGpsPosition();

    /**
     * @fn update
     * @brief Try to update the base position.
     * @param rvPosition The new position
     * @param vSlot      The attempt slot
     * @return true on success, else false
     * @threadsafe
     */
    bool update(object::Object&& rvPosition, std::size_t vSlot) override;

    /**
     * @see Data#registerSlot
     */
    std::size_t registerSlot() override;

private:
    /**
     * @fn isPositionGood
     * @brief Check whether the position is good enough.
     * @return true if yes, else false
     */
    bool isPositionGood();

    /// @var mPosition
    /// The position
    object::GpsPosition mPosition;

    /// @var mProcessor
    /// Processor for GPS information
    processor::GpsProcessor mProcessor;

    /// @var mPositionLocked
    /// Locking state of the current position
    bool mPositionLocked = false;

    /// @var mGroundMode
    /// Ground mode state
    bool mGroundMode = false;

    /// @var mAttempts
    /// Store update attempts
    std::vector<std::uint32_t> mAttempts;
};

}  // namespace data
