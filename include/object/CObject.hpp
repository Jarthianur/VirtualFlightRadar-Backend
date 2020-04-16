/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "util/class_utils.hpp"

#include "types.hpp"

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
    COPYABLE_BUT_NOT_MOVABLE(CObject)

    CTCONST OUTDATED = 4;  ///< Cycles until an object is considered outdated

    virtual ~CObject() noexcept = default;

    /// Increment the update age.
    CObject& operator++();

    /**
     * Try to update this object.
     * @param other_   The other object
     * @return true on success, else false
     */
    bool TryUpdate(CObject&& other_);

    GETTER(UpdateAge, m_updateAge)
};
}  // namespace vfrb::object
