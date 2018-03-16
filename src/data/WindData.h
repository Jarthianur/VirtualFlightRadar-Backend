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
#include "../object/Wind.h"

#include "Data.h"

/// @namespace data
namespace data
{
/**
 * @class WindData
 * @brief Store wind information.
 * @implements Data
 */
class WindData : public Data
{
public:
    DEFAULT_CTOR_DTOR(WindData)

    /**
     * @fn WindData
     * @brief Constructor
     * @param crWind The initial wind information
     */
    explicit WindData(const object::Wind& crWind);

    /**
     * @fn getSerialized
     * @brief Get the MWV sentence.
     * @note The wind info is invalid after this operation.
     * @return the MWV sentence, if valid, else empty string
     * @threadsafe
     */
    std::string getSerialized() override;

    /**
     * @fn update
     * @brief Attempt to update the wind information.
     * @param rvWind The new wind information.
     * @param vSlot  The attempt slot
     * @return true on success, else false
     * @threadsafe
     */
    bool update(object::Object&& rvWind, std::size_t vSlot) override;

    /**
     * @see Data#registerSlot
     */
    std::size_t registerSlot() override;

private:
    /// @var mWind
    /// The Wind information
    object::Wind mWind;

    /// @var mAttempts
    /// Store update attempts
    std::vector<std::uint32_t> mAttempts;
};

}  // namespace data
