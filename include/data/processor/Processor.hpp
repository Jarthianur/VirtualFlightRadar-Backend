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

#include <cstddef>
#include <cstdio>

namespace data
{
namespace processor
{
/**
 * @brief Processor base class/interface.
 * @tparam T The type of object to process
 */
template<typename T>
class Processor
{
public:
    Processor()                   = default;
    virtual ~Processor() noexcept = default;

    //< begin interfaces >//
    /**
     * @brief Process an object.
     * @param _1 The object of type T
     */
    virtual void process(T& _1) const = 0;
    //< end interfaces >//
};
}  // namespace processor
}  // namespace data
