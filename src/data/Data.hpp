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

#include <mutex>
#include <string>

namespace object
{
class Object;
}  // namespace object

namespace data
{
/**
 * @brief The Data interface
 */
class Data
{
public:
    /**
     * @brief Constructor
     */
    Data() = default;

    /**
     * @brief Destructor
     */
    virtual ~Data() noexcept = default;

    /**
     * @brief Get the serialized data.
     * @param dest The string to append the data to
     */
    virtual void get_serialized(std::string& dest) = 0;

    /**
     * @brief Attempt to update this data.
     * @param _1 The new Object
     * @return true on success, else false
     */
    virtual bool update(object::Object&& _1) = 0;

protected:
    /// Mutex for threadsafety
    mutable std::mutex m_mutex;
};
}  // namespace data
