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

#include "error/Error.hpp"

#include "types.h"

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
    IParser()                   = default;
    virtual ~IParser() noexcept = default;

    /**
     * Parse an object from string.
     * @param str_  The string to parse
     * @param prio_ The priority to pass
     * @return the parsed object
     * @throw vfrb::feed::parser::error::CParseError
     */
    virtual ObjectT Parse(Str&& str_, u32 prio_) const = 0;
};

namespace error
{
/// Error to indicate that unpacking failed
class CParseError : public vfrb::error::IError
{
public:
    CParseError()                    = default;
    ~CParseError() noexcept override = default;

    char const* Message() const noexcept override
    {
        return "";
    }
};
}  // namespace error
}  // namespace vfrb::feed::parser
