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

#include "error/IError.hpp"

namespace vfrb::util
{
namespace error
{
/// Error to indicate a value was not between required limits.
class CBoundsExceededError : public vfrb::error::IError
{
    String const m_msg;  ///< The error message

public:
    /**
     * @tparam T The value type
     * @param min_ The lower limit
     * @param max_ The upper limit
     */
    template<typename T>
    CBoundsExceededError(T val_, T min_, T max_)
        : m_msg(std::to_string(val_) + " not in [" + std::to_string(min_) + "," + std::to_string(max_) +
                "]") {}

    str Message() const noexcept override {
        return m_msg.c_str();
    }
};
}  // namespace error

/**
 * Check a value to be between given limits.
 * @tparam T The value type
 * @param val_ The value
 * @param min_ The lower limit
 * @param max_ The upper limit
 * @throw vfrb::util::error::CBoundsExceededError
 */
template<typename T>
void FailOutsideBounds(T val_, T min_, T max_) {
    if (val_ < min_ || val_ > max_) {
        throw error::CBoundsExceededError(val_, min_, max_);
    }
}
}  // namespace vfrb::util
