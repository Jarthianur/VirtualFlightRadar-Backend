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

#include "error/IError.hpp"
#include "util/ClassUtils.hpp"

#include "Types.hpp"

namespace vfrb::feed::parser
{
template<typename ObjectT>
class IParser
{
public:
    NOT_COPYABLE(IParser)
    NOT_MOVABLE(IParser)

    IParser()                   = default;
    virtual ~IParser() noexcept = default;

    virtual auto
    Parse(String&& str_, u32 prio_) const -> ObjectT = 0;
};

namespace error
{
class CParseError : public vfrb::error::IError
{
public:
    [[nodiscard]] auto
    Message() const noexcept -> str override {
        return "";
    }
};
}  // namespace error
}  // namespace vfrb::feed::parser
