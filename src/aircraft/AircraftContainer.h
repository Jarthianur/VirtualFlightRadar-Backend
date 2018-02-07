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

#ifndef SRC_AIRCRAFT_AIRCRAFTCONTAINER_H_
#define SRC_AIRCRAFT_AIRCRAFTCONTAINER_H_

#include <boost/thread/mutex.hpp>
#include <cstddef>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "../aircraft/AircraftProcessor.h"
#include "../config/Parameters.h"

#define AC_INVALIDATE         4
#define AC_DELETE_THRESHOLD   120
#define AC_NO_FLARM_THRESHOLD AC_INVALIDATE

namespace aircraft
{

class Aircraft;

/**
 * @class AircraftContainer
 * @brief Store Aircrafts and trigger processing.
 */
class AircraftContainer
{
public:
    /// Non-copyable
    AircraftContainer(const AircraftContainer&) = delete;
    /// Not assignable
    AircraftContainer& operator=(const AircraftContainer&) = delete;
    /**
     * @fn AircraftContainer
     * @brief Constructor
     */
    AircraftContainer();
    /**
     * @fn ~AircraftContainer
     * @brief Destructor
     */
    virtual ~AircraftContainer() noexcept;
    /**
     * @fn insertAircraft
     * @brief Insert an Aircraft into container.
     * @note May fail due to priority.
     * @param cr_update The Aircraft update
     * @param prio      The priority attempting to write
     * @threadsafe
     */
    void upsert(Aircraft& cr_update, std::uint32_t prio);
    /**
     * @fn processAircrafts
     * @brief Process all Aircrafts and get the reports as string.
     *        Increases update ages; "too old" Aircrafts are not reported and later deleted.
     * @see AircraftProcesser::process
     * @return the string with all NMEA reports
     * @threadsafe
     */
    std::string processAircrafts(const struct util::GpsPosition& crBasePos,
                                 double vAtmPress);

private:
    /**
     * @fn find
     * @brief Find an Aircraft by Id efficiently in the container using an index map.
     * @param cr_id The Id to search
     * @return an iterator to the Aircraft if found, else vector::end
     */
    std::vector<Aircraft>::iterator find(const std::string& cr_id);

    /// Mutex for threadsafety
    boost::mutex mMutex;
    /// Processor providing functionality to process Aircrafts
    AircraftProcessor mAcProc;
    /// Vector holding the Aircrafts
    std::vector<Aircraft> mCont;
    /// Index map to make find efficient
    std::unordered_map<std::string, size_t> mIndexMap;
};

}  // namespace aircraft

#endif /* SRC_AIRCRAFT_AIRCRAFTCONTAINER_H_ */
