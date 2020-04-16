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
#include "util/class_utils.hpp"

#include "types.hpp"

namespace vfrb::feed::parser
{
/**
 * Parser interface
 * @tparam ObjectT The object type to parse
 */
template<typename ObjectT>
class IParser
{
public:
    NOT_COPYABLE(IParser)

    IParser()                   = default;
    virtual ~IParser() noexcept = default;

    /**
     * Parse an object from string.
     * @param str_  The string to parse
     * @param prio_ The priority to pass
     * @return the parsed object
     * @throw vfrb::feed::parser::error::CParseError
     */
    virtual ObjectT Parse(String&& str_, u32 prio_) const = 0;
};

namespace error
{
/// Error to indicate that unpacking failed
class CParseError : public vfrb::error::IError
{
public:
    str Message() const noexcept override {
        return "";
    }
};
}  // namespace error
}  // namespace vfrb::feed::parser
