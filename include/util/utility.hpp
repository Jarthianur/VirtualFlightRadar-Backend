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

#include <type_traits>

#include "error/Error.hpp"

#include "class_utils.h"
#include "types.h"

namespace vfrb::util
{
namespace error
{
class CLimitsExceededError : public vfrb::error::IError
{
    Str const m_msg;

public:
    template<typename T>
    CLimitsExceededError(T val_, T min_, T max_)
        : m_msg(std::to_string(val_) + " not in [" + std::to_string(min_) + "," + std::to_string(max_) + "]")
    {}
    ~CLimitsExceededError() noexcept override = default;

    char const* What() const noexcept override
    {
        return m_msg.c_str();
    }
};
}  // namespace error

/**
 * @brief Get enum value as the underlying type.
 * @param value The enum value
 * @return the value as its underlyig type
 */
template<typename T>
constexpr auto EnumType(T value_) -> typename std::underlying_type<T>::type
{
    return static_cast<typename std::underlying_type<T>::type>(value_);
}

template<typename T>
void CheckLimits(T val_, T min_, T max_)
{
    if (val_ < min_ || val_ > max_)
    {
        throw error::CLimitsExceededError(val_, min_, max_);
    }
}
}  // namespace vfrb::util
