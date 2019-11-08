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

namespace vfrb::server::net::error
{
/**
 * @brief Exception to signal socket errors.
 */
class SocketError : public vfrb::error::Error
{
    Str const m_msg;  ///< Error message

public:
    SocketError() = default;
    explicit SocketError(Str const& msg);  ///< @param msg The error message
    ~SocketError() noexcept override = default;

    /**
     * @brief Get the error message.
     * @return the message
     */
    char const* what() const noexcept override;
};
}  // namespace vfrb::server::net::error
