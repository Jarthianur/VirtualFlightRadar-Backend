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

#include "util/defines.h"

/// @def AC_OUTDATED
/// Times until aircraft is outdated
#define OBJ_OUTDATED 4

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
    Object();

    /**
     * @fn Object
     * @brief Constructor
     * @param vPriority The initial priority
     */
    explicit Object(std::uint32_t priority);

    virtual ~Object() noexcept;

    /**
     * @fn tryUpdate
     * @brief Try to update this Object.
     * @note If the other Object cannot update this, nothing happens.
     * @see canUpdate
     * @param rvOther   The other Object
     * @param vAttempts The update attempt count
     * @return true on success, else false
     */
    virtual bool tryUpdate(Object&& other);

    /**
     * @fn setSerialized
     * @brief Set the string representation of this Objects data.
     * @param rvSerialized The string representation
     */
    virtual void set_serialized(std::string&& serialized);

    /**
     * @fn getSerialized
     * @brief Get the string representation of this Objects data.
     * @return mSerialized
     */
    virtual const std::string& get_serialized() const;

    Object& operator++();

protected:
    /**
     * @fn assign
     * @brief Assign other objects values to this.
     * @param rvOther The other Object
     */
    virtual void assign(Object&& other);

    /**
     * @fn canUpdate
     * @brief Check whether this Object can update the other one.
     * @param crOther   The other Object
     * @param vAttempts The update attempt count
     * @return true if yes, else false
     */
    virtual bool canUpdate(const Object& other) const;

    /// @var mLastPriority
    /// Got last update with this priority.
    std::uint32_t m_lastPriority;

    /// @var mSerialized
    /// The string representation of this Objects data.
    std::string m_serialized;

    /// @var mUpdateAge
    /// Times processed without update.
    std::uint32_t m_updateAge = 0;

public:
    GETTER_V(updateAge)
};
}  // namespace object
