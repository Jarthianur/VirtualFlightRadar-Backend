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

#include "types.h"

namespace vfrb::object
{
/// Base class for objects
class CObject
{
protected:
    u32 m_lastPriority = 0;  ///< Got last update with this priority
    u32 m_updateAge    = 0;  ///< Times processed without update

    CObject() = default;

    /// @param prio_ The initial priority
    explicit CObject(u32 prio_);

    /**
     * Assign other objects values to this.
     * @param other_ The other object
     */
    virtual void assign(CObject&& other_);

    /**
     * Check whether this object can update an other one.
     * @param other_   The other object
     * @return true if yes, else false
     */
    virtual bool canUpdate(CObject const& other_) const;

public:
    inline static constexpr auto OUTDATED = 4;  ///< Cycles until an object is considered outdated

    virtual ~CObject() noexcept = default;

    /// Increment the update age.
    CObject& operator++();

    /**
     * Try to update this object.
     * @param other_   The other object
     * @return true on success, else false
     */
    bool TryUpdate(CObject&& other_);

    auto UpdateAge() const -> decltype(m_updateAge);
};
}  // namespace vfrb::object
