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

#include "util/ClassUtils.hpp"

#include "Types.hpp"

namespace vfrb::object
{
class CObject
{
protected:
    u32 m_lastPriority = 0;
    u32 m_updateAge    = 0;

    CObject() = default;

    explicit CObject(u32 prio_);

    virtual void
    assign(CObject&& other_);

    [[nodiscard]] virtual auto
    canUpdate(CObject const& other_) const -> bool;

public:
    DEFAULT_COPYABLE(CObject)
    COPY_ON_MOVE(CObject)

    CTCONST OUTDATED = 4U;

    virtual ~CObject() noexcept = default;

    auto
    operator++() -> CObject&;

    auto
    TryUpdate(CObject&& other_) -> bool;

    GETTER(UpdateAge, m_updateAge)
};
}  // namespace vfrb::object
