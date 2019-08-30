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
 * @brief Interface for parsers.
 * @tparam T The corresponding object type
 */
template<typename ObjectT>
class Parser
{
public:
    Parser()                   = default;
    virtual ~Parser() noexcept = default;

    /**
     * @brief Unpack a given string into the templated object.
     * @param sentence The string to unpack
     * @param _1       The target object
     * @return true on success, else false
     */
    virtual ObjectT unpack(str const& sentence, u32 priority) const = 0;
};

namespace error
{
class UnpackError : public vfrb::error::Error
{
public:
    UnpackError()                    = default;
    ~UnpackError() noexcept override = default;
    char const* what() const noexcept override
    {
        return "";
    }
};
}  // namespace error
}  // namespace vfrb::feed::parser
