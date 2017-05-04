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

#include <boost/thread/mutex.hpp>
#include <cstdint>
#include <string>

#include "../util/Priority.h"

/**
 * Data structure.
 *
 * Hold a value providing its own mutex and meta-data.
 *
 * @tparam T the type of value to hold
 */
template<typename T>
struct Data
{
    /// The value
    T value;
    /// Is wrtite attempt valid?
    bool attemptValid;
    /// Last written Priority
    Priority lastPriority;
    /// Mutex to enable threadsafety per Data.
    boost::mutex mutex;
    /**
     * Update the value.
     * May fail due to Priority.
     * Updates always if attempt is valid.
     *
     * @param cr_nv   the new value
     * @param prio the Priority attempting to write
     */
    void update(const T& cr_nv, Priority prio)
    {
        bool write = attemptValid;
        if (!write)
        {
            if (prio > lastPriority || (prio == lastPriority
                    && prio != Priority::LESSER))
            {
                write = true;
            }
        }
        if (write)
        {
            attemptValid = (prio == Priority::LESSER);
            value = cr_nv;
            lastPriority = prio;
        } else
        {
            attemptValid = true;
        }
    }
};
/**
 * Pseudo temporary data structure.
 *
 * Same as the Data structure, but reading the value
 * invalidates it and updating validates it.
 *
 * @tparam T the type of value to hold
 */
template<typename T>
struct TmpData
{
    /// The value
    T value;
    /// Is wrtite attempt valid?
    bool attemptValid;
    /// Is the value valid?
    bool valueValid;
    /// Last written Priority
    Priority lastPriority;
    /// Mutex to enable threadsafety per TmpData.
    boost::mutex mutex;
    /**
     * Get the value and invalidate it.
     *
     * @return the value
     */
    const T& getValue()
    {
        valueValid = false;
        return value;
    }
    /**
     * Update the value.
     * May fail due to Priority.
     * Updates always if attempt is valid.
     * Validates the value.
     *
     * @param cr_nv   the new value
     * @param prio the Priority attempting to write
     */
    void update(const T& cr_nv, Priority prio)
    {
        bool write = attemptValid;
        if (!write)
        {
            if (prio > lastPriority || (prio == lastPriority
                    && prio != Priority::LESSER))
            {
                write = true;
            }
        }
        if (write)
        {
            attemptValid = (prio == Priority::LESSER);
            value = cr_nv;
            lastPriority = prio;
            valueValid = true;
        } else
        {
            attemptValid = true;
        }
    }
};

#endif /* SRC_DATA_DATA_HPP_ */
