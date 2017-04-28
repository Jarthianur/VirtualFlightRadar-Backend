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

template<typename T>
struct Data
{
    T value;
    bool attemptValid;
    Priority lastPriority;
    boost::mutex mutex;

    void update(const T& nv, Priority prio)
    {
        bool write = attemptValid;
        if (!write)
        {
            if (prio > lastPriority || (prio == lastPriority && prio != Priority::LESSER))
            {
                write = true;
            }
        }
        if (write)
        {
            attemptValid = (prio == Priority::LESSER);
            value = nv;
            lastPriority = prio;
        }
        else
        {
            attemptValid = true;
        }
    }
};

template<typename T>
struct TmpData
{
    T value;
    bool attemptValid;
    bool valueValid;
    Priority lastPriority;
    boost::mutex mutex;

    void update(const T& nv, Priority prio)
    {
        bool write = attemptValid;
        if (!write)
        {
            if (prio > lastPriority || (prio == lastPriority && prio != Priority::LESSER))
            {
                write = true;
            }
        }
        if (write)
        {
            attemptValid = (prio == Priority::LESSER);
            value = nv;
            lastPriority = prio;
            valueValid = true;
        }
        else
        {
            attemptValid = true;
        }
    }
};

#endif /* SRC_DATA_DATA_HPP_ */
