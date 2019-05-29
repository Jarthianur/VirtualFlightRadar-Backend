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

#include "util/utility.hpp"

namespace object
{
/**
 * @brief Base object class
 */
class Object
{
protected:
    //< begin members >//
    std::uint32_t m_lastPriority = 0;  ///< Got last update with this priority.
    std::uint32_t m_updateAge    = 0;  ///< Times processed without update.
    //< end members >//

    //< begin methods >//
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
    virtual bool canUpdate(const Object& other) const;
    //< end methods >//

public:
    Object() = default;
    explicit Object(std::uint32_t priority);  ///< @param priority The initial priority
    virtual ~Object() noexcept = default;

    //< begin operators >//
    /**
     * @brief Increment the update age.
     * @return this
     */
    Object& operator++();
    //< end operators >//

    //< begin interfaces >//
    /**
     * @brief Try to update this Object.
     * @note If the other Object cannot update this, nothing happens.
     * @param other   The other Object
     * @return true on success, else false
     */
    virtual bool tryUpdate(Object&& other);

    virtual util::CStringPack getNMEA() const = 0;
    auto                      getUpdateAge() const -> decltype(m_updateAge);
    //< end interfaces >//

    //< begin constants >//
    static constexpr const std::uint32_t OUTDATED = 4;
    //< end constants >//
};
}  // namespace object
