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

#include "util/types.h"
#include "util/utility.hpp"

namespace vfrb::object
{
/**
 * @brief Base object class
 */
class Object
{
protected:
    u32 m_lastPriority = 0;  ///< Got last update with this priority.
    u32 m_updateAge    = 0;  ///< Times processed without update.

    Object() = default;
    explicit Object(u32 priority);  ///< @param priority The initial priority

    /**
     * @brief Assign other objects values to this.
     * @param other The other Object
     */
    virtual void assign(Object&& other);

    /**
     * @brief Check whether this Object can update the other one.
     * @param other   The other Object
     * @return true if yes, else false
     */
    virtual bool canUpdate(Object const& other) const;

public:
    inline static constexpr auto OUTDATED = 4;

    virtual ~Object() noexcept = default;

    /**
     * @brief Increment the update age.
     * @return this
     */
    Object& operator++();

    /**
     * @brief Try to update this Object.
     * @note If the other Object cannot update this, nothing happens.
     * @param other   The other Object
     * @return true on success, else false
     */
    virtual bool tryUpdate(Object&& other);

    virtual std::string_view nmea() const = 0;
    auto                     updateAge() const -> decltype(m_updateAge);
};
}  // namespace vfrb::object
