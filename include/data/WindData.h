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
#include "util/defines.h"

#include "Data.hpp"

namespace data
{
/**
 * @brief Store wind information.
 */
class WindData : public Data
{
public:
    DEFAULT_DTOR(WindData)

    WindData();

    /**
     * @brief Constructor
     * @param wind The initial wind information
     */
    explicit WindData(const object::Wind& wind);

    /**
     * @brief Get the MWV sentence.
     * @note The wind info is invalid after this operation.
     * @param dest The destination string to append data
     * @threadsafe
     */
    void get_serialized(std::string& dest) override;

    /**
     * @brief Update the wind information.
     * @param wind The new wind information.
     * @return true on success, else false
     * @threadsafe
     */
    bool update(object::Object&& wind) override;

private:
    /// The Wind information
    object::Wind m_wind;
};

}  // namespace data
