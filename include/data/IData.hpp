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

#include <functional>
#include <utility>

#include "util/ClassUtils.hpp"

#include "Types.hpp"

namespace vfrb::object
{
class CObject;
}  // namespace vfrb::object

namespace vfrb::data
{
struct SAccessor
{
    object::CObject const& Obj;
    StringView             Nmea;
};

using AccessFn = std::function<void(SAccessor const&)>;

/**
 * @brief The Data interface
 */
class IData
{
protected:
    AccessFn m_accessFn;

public:
    NOT_COPYABLE(IData)
    NOT_MOVABLE(IData)

    explicit IData(AccessFn&& fn_) : m_accessFn(std::move(fn_)) {}
    virtual ~IData() noexcept = default;

    /**
     * @brief Attempt to update this data.
     * @param _1 The new Object
     * @return true on success, else false
     */
    virtual auto Update(object::CObject&& obj_) -> bool = 0;

    virtual void Access() = 0;
};
}  // namespace vfrb::data
