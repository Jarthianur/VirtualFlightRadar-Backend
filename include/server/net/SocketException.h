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

#include <exception>
#include <string>

#include "util/defines.h"

namespace server
{
namespace net
{
/**
 * @brief Exception to signal socket errors.
 */
class SocketException : public std::exception
{
public:
    DEFAULT_DTOR(SocketException)

    /**
     * @brief Constructor
     * @param msg The error message
     */
    explicit SocketException(const std::string& msg);

    /**
     * @brief Get the error message.
     * @return the message
     */
    const char* what() const noexcept;

private:
    /// Error message
    const std::string m_message;
};
}  // namespace net
}  // namespace server
