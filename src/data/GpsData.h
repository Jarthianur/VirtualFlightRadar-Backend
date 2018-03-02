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

#include "Data.h"

#include "object/Position.h"
#include "processor/GpsProcessor.h"

/// @namespace data
namespace data
{
/**
 * @class GpsData
 * @brief Manage GPS information.
 */
class GpsData : public Data
{
public:
    GpsData();

    /**
     * @fn GpsData
     * @brief Constructor
     * @param vPosition The initial info
     */
    explicit GpsData(const object::ExtGpsPosition& crPosition);

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
    std::string getSerialized() override;

    /**
     * @fn getBasePos
     * @brief Get the base GPS position.
     * @return the position
     * @threadsafe
     */
    object::GpsPosition getGpsPosition();

    /**
     * @fn update
     * @brief Try to update the base position.
     * @param crPosition The new position
     * @param vPriority  The attempts priority
     * @param rAttempts  The update attempts
     */
    bool update(const object::Object& crPosition, std::size_t vSlot) override;

private:
    /// @var mBasePos
    /// Wrapper holding the base position
    object::ExtGpsPosition mBasePos;

    /// @var mGpsModule
    /// GpsModule providing functionality to build GPS sentences
    processor::GpsProcessor mProcessor;

    /// @var mPosLocked
    /// Locking state of the current position
    bool mPosLocked = false;
};

}  // namespace data
