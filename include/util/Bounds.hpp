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

namespace vfrb::util
{
namespace error
{
class CBoundsExceededError : public vfrb::error::IError
{
    String const m_msg;

public:
    template<typename T>
    CBoundsExceededError(T val_, T min_, T max_)
        : m_msg(std::to_string(val_) + " not in [" + std::to_string(min_) + "," + std::to_string(max_) +
                "]") {}

    [[nodiscard]] auto
    Message() const noexcept -> str override {
        return m_msg.c_str();
    }
};
}  // namespace error

template<typename T>
void
FailOutsideBounds(T val_, T min_, T max_) {
    if (val_ < min_ || val_ > max_) {
        throw error::CBoundsExceededError(val_, min_, max_);
    }
}
}  // namespace vfrb::util
