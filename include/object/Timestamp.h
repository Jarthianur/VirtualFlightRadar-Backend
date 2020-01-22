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

namespace vfrb::object
{
namespace error
{
/// Error to indicate timestamp parsing failure.
class CTimestampParseError : public vfrb::error::IError
{
public:
    CTimestampParseError()                    = default;
    ~CTimestampParseError() noexcept override = default;

    char const* Message() const noexcept override;
};
}  // namespace error

/// A self parsing Timestamp
class CTimestamp
{
    s64 m_value = 0;  ///< Time in milliseconds
    u32 m_day   = 0;  ///< Incremental day number

public:
    CTimestamp() = default;

    /**
     * @param sv_ The time string
     * @throw vfrb::object::error::CTimestampParseError
     */
    CTimestamp(StrView const& sv_);
    CTimestamp(CTimestamp const& other_);
    ~CTimestamp() noexcept = default;

    CTimestamp& operator=(CTimestamp const& other_);

    bool operator>(CTimestamp const& other_) const;
};
}  // namespace vfrb::object
