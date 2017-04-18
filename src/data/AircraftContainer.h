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

#ifndef SRC_DATA_AIRCRAFTCONTAINER_H_
#define SRC_DATA_AIRCRAFTCONTAINER_H_

#include <boost/thread/mutex.hpp>
#include <cstddef>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "../aircraft/Aircraft.h"
#include "../aircraft/AircraftProcessor.h"
#include "../util/Priority.h"
#include "../config/Parameters.h"

#define AC_NOT_FOUND -1
#define AC_INVALIDATE AIRCRAFT_INVALIDATE
#define AC_DELETE_THRESHOLD 120
#define AC_NO_FLARM_THRESHOLD 4

class AircraftContainer
{
public:
    AircraftContainer(const AircraftContainer&) = delete;
    AircraftContainer& operator=(const AircraftContainer&) = delete;

    AircraftContainer();
    virtual ~AircraftContainer() noexcept;

    /**
     * Insert aircraft in container, prefer FLARM.
     */
    void insertAircraft(const Aircraft& /*update*/, Priority /*prio*/);

    /**
     * Process all aircrafts.
     */
    std::string processAircrafts();

private:
    /**
     * search aircraft in container by given id.
     * if id is found returns index,
     * else returns -1.
     */
    ssize_t find(const std::string& /*id*/);

    boost::mutex mMutex;

    /**
     * Processor to process Aircrafts from
     * within threadsafe context of container.
     */
    AircraftProcessor mAcProc;

    /**
     * Container that holds Aircrafts
     */
    std::vector<Aircraft> mCont;

    /**
     * Map to manage indices of Aircrafts.
     * This makes find-method much more efficient.
     */
    std::unordered_map<std::string, size_t> mIndexMap;

    Priority mLastPriority = Priority::DONTCARE;
    bool mInputValid = false;
};

#endif /* SRC_DATA_AIRCRAFTCONTAINER_H_ */
