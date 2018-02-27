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

#include <cstdint>
#include <string>
#include <type_traits>

#include "Object.h"

namespace data
{
namespace object
{
/**
 * @class Wrapper
 * @brief Store any value and provide per Wrapper meta-data and mutex.
 * @tparam T The type of value to hold
 */
template<typename T,
         typename std::enable_if<std::is_base_of<Object, T>::value>::type* = nullptr>
struct Wrapper
{
    /**
     * @fn trySetValue
     * @brief Try to set the value, fails if attempts priority is less.
     * @note Succeeds always if attempt is valid.
     * @param cr_nv The new value
     * @param prio  The attempts priority
     */
    inline bool trySetValue(const T& crNewValue, std::uint64_t& rAttempts)
    {
        mUpdated = (crNewValue.getLastPriority() >= mValue.getLastPriority())
                   || (!mUpdated
                       && crNewValue.getLastPriority() * ++rAttempts
                              >= mValue.getLastPriority());
        if(mUpdated)
        {
            mValue = crNewValue;
        }
        return mUpdated;
    }

    /**
     * @fn getValue
     * @brief Get the value.
     * @return the value
     */
    inline const Object& getValue()
    {
        return mValue;
    }

private:
    /// The value
    T mValue;

    ///
    bool mUpdated = false;
};

/**
 * @class TmpWrapper
 * @brief Pseudo temporary Wrapper; value is invalid after read and valid after write.
 * @tparam T the type of value to hold
 */
template<typename T,
         typename std::enable_if<std::is_base_of<Object, T>::value>::type* = nullptr>
struct TmpWrapper
{
    /// Is the value valid?
    bool isValueValid = false;

    /**
     * @fn trySetValue
     * @brief Try to set the value, fails if attempts priority is less.
     * @note Succeeds always if attempt is valid.
     * @param cr_nv The new value
     * @param prio  The attempts priority
     */
    inline bool trySetValue(const T& crNewValue, std::uint64_t& rAttempts)
    {
        mUpdated = (crNewValue.getLastPriority() >= mValue.getLastPriority())
                   || (!mUpdated
                       && crNewValue.getLastPriority() * ++rAttempts
                              >= mValue.getLastPriority());
        if(mUpdated)
        {
            mValue       = crNewValue;
            isValueValid = true;
        }
        return mUpdated;
    }

    /**
     * @fn getValue
     * @brief Get the value and invalidate it.
     * @return the value
     */
    inline const T& getValue()
    {
        isValueValid = false;
        return mValue;
    }

private:
    /// The value
    T mValue;

    ///
    bool mUpdated = false;
};

}  // namespace util
}
