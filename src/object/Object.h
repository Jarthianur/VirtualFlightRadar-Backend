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

#include <cstdint>
#include <string>

#include "../Defines.h"

/// @namespace object
namespace object
{
/**
 * @class Object
 * @brief Base object class
 *
 * This is a basic representation of an object used throughout this software.
 */
class Object
{
public:
    DEFAULT_CTOR_DTOR(Object)

    /**
     * @fn Object
     * @brief Constructor
     * @param vPriority The initial priority
     */
    explicit Object(std::uint32_t vPriority);

    /**
     * @fn tryUpdate
     * @brief Try to update this Object.
     * @note If the other Object cannot update this, nothing happens.
     * @see canUpdate
     * @param rvOther   The other Object
     * @param vAttempts The update attempt count
     * @return true on success, else false
     */
    virtual bool tryUpdate(Object&& rvOther, std::uint32_t vAttempts);

    /**
     * @fn setSerialized
     * @brief Set the string representation of this Objects data.
     * @param rvSerialized The string representation
     */
    virtual void setSerialized(std::string&& rvSerialized);

    /**
     * @fn getSerialized
     * @brief Get the string representation of this Objects data.
     * @return mSerialized
     */
    virtual const std::string& getSerialized() const;

protected:
    /**
     * @fn assign
     * @brief Assign other objects values to this.
     * @param rvOther The other Object
     */
    virtual void assign(Object&& rvOther);

    /**
     * @fn canUpdate
     * @brief Check whether this Object can update the other one.
     * @param crOther   The other Object
     * @param vAttempts The update attempt count
     * @return true if yes, else false
     */
    virtual bool canUpdate(const Object& crOther, std::uint32_t vAttempts) const;

    /// @var mLastPriority
    /// Got last update with this priority.
    std::uint32_t mLastPriority;

    /// @var mSerialized
    /// The string representation of this Objects data.
    std::string mSerialized;
};
}  // namespace object