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

    int vecfind(std::string&);
    void pushAircraft(Aircraft*);
    Aircraft* getAircraft(int i);
    void invalidateAircrafts();
    std::vector<Aircraft*>& getContainer();

private:
    std::mutex mutex;
    std::vector<Aircraft*> vec;
};

#endif /* AIRCRAFTCONTAINER_H_ */
