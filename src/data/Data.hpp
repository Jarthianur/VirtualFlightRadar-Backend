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

#ifndef SRC_DATA_DATA_HPP_
#define SRC_DATA_DATA_HPP_

#include <cstdint>

namespace data
{

/**
 * @class Data
 * @brief An interface for data containers.
 * @tparam T The type of data to manage
 */
template<typename T>
class Data
{
public:
    /**
     * @fn Data
     * @brief Constructor
     */
    inline Data()
    {
    }
    /**
     * @fn ~Data
     * @brief Destructor
     */
    inline virtual ~Data() noexcept
    {
    }
    /**
     * @fn update
     * @brief Update the specialized data.
     * @note To be implemented.
     * @tparam T The new data
     * @param prio The priority of the update
     */
    virtual void update(const T&, std::int32_t prio) = 0;
    /**
     * @fn init
     * @brief Initialize data.
     * @note To be implemented.
     * @tparam T The initial data
     */
    virtual void init(T) = 0;
};

} /* namespace data */

#endif /* SRC_DATA_DATA_HPP_ */
