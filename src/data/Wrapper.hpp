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

#ifndef SRC_DATA_WRAPPER_HPP_
#define SRC_DATA_WRAPPER_HPP_

#include <boost/thread/mutex.hpp>
#include <cstdint>
#include <string>

namespace data
{

/**
 * @class Wrapper
 * @brief Store any value and provide per Wrapper meta-data and mutex.
 * @tparam T The type of value to hold
 */
template<typename T>
struct Wrapper
{
    /// The value
    T value;
    /// Is wrtite attempt valid?
    bool attemptValid = true;
    /// Last written priority
    std::int32_t lastPriority = 0;
    /// Mutex to enable threadsafety per Wrapper.
    boost::mutex mutex;
    /**
     * @fn trySetValue
     * @brief Try to set the value, fails if attempts priority is less.
     * @note Succeeds always if attempt is valid.
     * @param cr_nv The new value
     * @param prio  The attempts priority
     */
    void trySetValue(const T& cr_nv, std::int32_t prio)
    {
        bool write = attemptValid;
        if (!write)
        {
            if (prio >= lastPriority)
            {
                write = true;
            }
        }
        if (write)
        {
            attemptValid = false;
            value = cr_nv;
            lastPriority = prio;
        } else
        {
            attemptValid = true;
        }
    }
};
/**
 * @class TmpWrapper
 * @brief Pseudo temporary Wrapper; value is invalid after read and valid after write.
 * @tparam T the type of value to hold
 */
template<typename T>
struct TmpWrapper
{
    /// The value
    T value;
    /// Is wrtite attempt valid?
    bool attemptValid;
    /// Is the value valid?
    bool valueValid;
    /// Last written priority
    std::int32_t lastPriority;
    /// Mutex to enable threadsafety per TmpWrapper.
    boost::mutex mutex;
    /**
     * @fn getValue
     * @brief Get the value and invalidate it.
     * @return the value
     */
    const T& getValue()
    {
        valueValid = false;
        return value;
    }
    /**
     * @fn trySetValue
     * @brief Try to set the value, fails if attempts priority is less.
     * @note Succeeds always if attempt is valid.
     * @param cr_nv The new value
     * @param prio  The attempts priority
     */
    void trySetValue(const T& cr_nv, std::int32_t prio)
    {
        bool write = attemptValid;
        if (!write)
        {
            if (prio >= lastPriority)
            {
                write = true;
            }
        }
        if (write)
        {
            attemptValid = false;
            value = cr_nv;
            lastPriority = prio;
            valueValid = true;
        } else
        {
            attemptValid = true;
        }
    }
};

}  // namespace data

#endif /* SRC_DATA_WRAPPER_HPP_ */
