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

#include "object/Wind.h"

#include "Data.hpp"

namespace data
{
/**
 * @brief Store wind information.
 */
class WindData : public Data
{
public:
    /**
     * @brief Constructor
     */
    WindData();

    /**
     * @brief Constructor
     * @param wind The initial wind information
     */
    explicit WindData(const object::Wind& wind);

    /**
     * @brief Destructor
     */
    ~WindData() noexcept = default;

    /**
     * @fn getSerialized
     * @brief Get the MWV sentence.
     * @note The wind info is invalid after this operation.
     * @return the MWV sentence, if valid, else empty string
     * @threadsafe
     */
    void get_serialized(std::string& dest) override;

    /**
     * @fn update
     * @brief Attempt to update the wind information.
     * @param rvWind The new wind information.
     * @param vSlot  The attempt slot
     * @return true on success, else false
     * @threadsafe
     */
    bool update(object::Object&& wind) override;

private:
    /// @var mWind
    /// The Wind information
    object::Wind m_wind;
};

}  // namespace data
