/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

#ifndef AIRCRAFTCONTAINER_H_
#define AIRCRAFTCONTAINER_H_

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "../aircraft/Aircraft.h"
#include "../aircraft/AircraftProcessor.h"
#include "../config/Parameters.h"


#define AC_NOT_FOUND -1
#define AC_INVALIDATE AIRCRAFT_INVALIDATE

class AircraftContainer
{
public:
    AircraftContainer(const AircraftContainer&) = delete;
    AircraftContainer& operator=(const AircraftContainer&) = delete;

    AircraftContainer();
    virtual ~AircraftContainer() throw ();

    void initProcessor(double, double, int32_t, double);

    /**
     * insert aircraft in container
     */
    void insertAircraft(std::string&, double, double, int32_t);
    void insertAircraft(std::string&, double, double, int32_t, double, uint32_t, int32_t,
            double, double, double);

    /**
     * process aircraft at index i into target string,
     * if index i is valid.
     */
    std::string processAircrafts();

private:
    /**
     * search aircraft in container by given id.
     * if id is found returns index,
     * else returns -1.
     */
    ssize_t find(std::string&);

    std::mutex mutex;

    /**
     * Processor to process Aircrafts from
     * within threadsafe context of container.
     */
    AircraftProcessor proc;

    /**
     * Container that holds Aircrafts
     */
    std::vector<Aircraft> cont;

    /**
     * Map to manage indices of Aircrafts.
     * This makes find-method much more efficient.
     */
    std::unordered_map<std::string, size_t> index_map;
};

#endif /* AIRCRAFTCONTAINER_H_ */
