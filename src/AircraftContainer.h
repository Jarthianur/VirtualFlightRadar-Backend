/*
 * AircraftContainer.h
 *
 *  Created on: 01.05.2016
 *      Author: lula
 */

#ifndef AIRCRAFTCONTAINER_H_
#define AIRCRAFTCONTAINER_H_

#include "Aircraft.h"
#include <mutex>
#include <vector>

#define INVALIDATE 4

class AircraftContainer
{
public:
    AircraftContainer();
    virtual ~AircraftContainer();

    /**
     * All methods in this class are threadsafe and exceptionsafe!
     */

    /**
     * search aircraft in container by given id.
     * if id is found returns index,
     * else returns -1.
     */
    int find(std::string&);

    /**
     * insert aircraft in container
     */
    void pushAircraft(Aircraft*);

    /**
     * returns pointer, to aircraft, at index i,
     * if index i is valid, else nullpointer.
     */
    Aircraft* getAircraft(unsigned int i);

    /**
     * increments all aircrafts's valid counter in container.
     * if valid counter is >= INVALIDATE, the aircraft is deleted from
     * container and it's memory is deallocated.
     *
     * this method also guarantees that all aircrafts will be deallocated
     * sooner or later.
     */
    void invalidateAircrafts();

    /**
     * return container's size
     */
    unsigned int getContSize();

private:
    std::mutex mutex;
    std::vector<Aircraft*> vec;
};

#endif /* AIRCRAFTCONTAINER_H_ */
