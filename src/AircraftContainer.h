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
     * returns pointer, to aircraft, at index i,
     * if index i is valid, else nullpointer.
     */
    Aircraft& getAircraft(unsigned int i);

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
