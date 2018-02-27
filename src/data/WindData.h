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

#include "object/Sensor.h"
#include "object/Wrapper.hpp"

/// @namespace data
namespace data
{
/**
 * @class WindData
 * @brief Manage sensor information.
 */
class WindData : public Data
{
public:
    /**
     * @fn WindData
     * @brief Constructor
     */
    WindData();

    explicit WindData(const object::Wind& crWind);

    /**
     * @fn ~WindData
     * @brief Destructor
     */
    virtual ~WindData() noexcept;

    /**
     * @fn getMwvStr
     * @brief Get the MWV sentence.
     * @note MWV is invalid after this operation.
     * @return the MWV sentence, if valid, else empty string
     * @threadsafe
     */
    std::string getSerialized() override;

    /**
     * @fn update
     * @brief Try to update the sensor information.
     * @param crWind    The new wind information.
     * @param vPriority The attempts priority
     * @param rAttempts The update attempts
     */
    bool update(const object::Object& crWind, std::uint64_t& rAttempts) override;

private:
    /// @var mWind
    /// Holding MDA sentence
    object::Wind mWind;
    bool isValueValid = false;
};

}  // namespace data
