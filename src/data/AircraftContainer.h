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

#include "../aircraft/AircraftProcessor.h"
#include "../util/Priority.h"
#include "../config/Parameters.h"

namespace aircraft
{
class Aircraft;
}

namespace data
{

/**
 * The AircraftContainer class.
 *
 * This class holds all registered Aircrafts and provides
 * functionality to process these into NMEA sentences.
 * Sentences are PFLAU and PFLAA.
 */
class AircraftContainer
{
public:
    /// Non-copyable
    AircraftContainer(const AircraftContainer&) = delete;
    /// Not assignable
    AircraftContainer& operator=(const AircraftContainer&) = delete;
    /**
     * Constructor
     */
    AircraftContainer();
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~AircraftContainer() noexcept;
    /**
     * Insert an Aircraft into container.
     * Handles FLARM preferation.
     * May fail due to Priority.
     *
     * @param cr_update the Aircraft update
     * @param prio the Priority attempting to write
     *
     * @exceptsafe strong
     */
    void insertAircraft(const aircraft::Aircraft& /*cr_update*/,
                        util::Priority /*prio*/) noexcept;
    /**
     * Process all aircrafts into NMEA sentences PFLAU and PFLAA.
     * Aircrafts with too old information are not reported, later deleted.
     * Resulting sentences contain trailing <cr><lf>.
     *
     * @return the string with all NMEA sentences
     *
     * @exceptsafe no-throw
     */
    std::string processAircrafts() noexcept;

private:
    /**
     * Find an Aircraft by ID efficiently in the container with index map.
     *
     * @param cr_id the ID to search
     *
     * @return an iterator to the Aircraft if found, else vector::end
     */
    std::vector<aircraft::Aircraft>::iterator find(
            const std::string& /*cr_id*/);

    /// Mutex for threadsafety
    boost::mutex mMutex;
    /// Processor providing functionality to process Aircrafts
    aircraft::AircraftProcessor mAcProc;
    /// Vector holding the Aircrafts
    std::vector<aircraft::Aircraft> mCont;
    /// Index map to make find efficient
    std::unordered_map<std::string, size_t> mIndexMap;
};

}  // namespace data

#endif /* SRC_DATA_AIRCRAFTCONTAINER_H_ */
