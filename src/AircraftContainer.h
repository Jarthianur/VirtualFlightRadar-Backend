/*
Copyright_License {

  Copyright (C) 2016 Virtual Flight Radar - Backend
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

#include "AircraftProcessor.h"
#include <mutex>
#include <vector>
#include <unordered_map>

#define INVALIDATE 4

class AircraftContainer
{
public:
    AircraftContainer();
    virtual ~AircraftContainer() throw();

    /**
     * All methods in this class are threadsafe
     */

    /**
     * insert aircraft in container
     */
    void insertAircraft(long double, long double, int, std::string&);
    void insertAircraft(long double, long double, int, std::string&, int, int, int, int, float);

    /**
     * process aircraft at index i into target string,
     * if index i is valid.
     */
    void processAircraft(unsigned int, std::string&);

    /**
     * increments all aircrafts's valid counter in container.
     * if valid counter is >= INVALIDATE, the aircraft is deleted from
     * container
     */
    void invalidateAircrafts();

    /**
     * return container's size
     */
    unsigned int getContSize();

    /**
     * deallocate all aircrafts and clear container
     */
    void clear();

private:
    /**
     * search aircraft in container by given id.
     * if id is found returns index,
     * else returns -1.
     */
    int find(std::string&);

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
    std::unordered_map<std::string,int> index_map;
};

#endif /* AIRCRAFTCONTAINER_H_ */
