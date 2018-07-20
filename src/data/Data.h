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
#include <mutex>

/// @namespace object
namespace object
{
class Object;
}  // namespace object

/// @namespace data
namespace data
{
/**
 * @class Data
 * @brief The Data interface.
 */
class Data
{
public:
    Data();

    virtual ~Data() noexcept;

    /**
     * @fn getSerialized
     * @brief Get the serialized data.
     * @return the string
     */
    virtual std::string getSerialized() = 0;

    /**
     * @fn update
     * @brief Attempt to update this data.
     * @param _1    The new Object
     * @param vSlot The slot for registered attempts
     * @return true on success, else false
     */
    virtual bool update(object::Object&& _1) = 0;

protected:
    /// @var mMutex
    /// Used for RW on this data
    std::mutex mMutex;
};
}  // namespace data
