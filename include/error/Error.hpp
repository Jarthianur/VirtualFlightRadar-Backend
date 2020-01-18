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

namespace vfrb::error
{
/// Interface for a generic exception type.
class IError : public std::exception
{
    inline char const* what() const noexcept override
    {
        return Message();
    }

public:
    IError()                            = default;
    virtual ~IError() noexcept override = default;

    /**
     * Get the message for this error.
     * @return the error message
     */
    virtual char const* Message() const noexcept = 0;
};
}  // namespace vfrb::error
