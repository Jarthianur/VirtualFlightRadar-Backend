/*
 * AircraftContainer.cpp
 *
 *  Created on: 01.05.2016
 *      Author: lula
 */

#include "AircraftContainer.h"

AircraftContainer::AircraftContainer()
{
}

AircraftContainer::~AircraftContainer()
{
}

int AircraftContainer::find(std::string& id)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    unsigned int i;
    for (i = 0; i < vec.size(); ++i) {
        if (vec.at(i)->id.compare(id) == 0) {
            return i;
        }
    }
    return -1;
}

void AircraftContainer::pushAircraft(Aircraft* ac)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    vec.push_back(ac);
    return;
}

void AircraftContainer::invalidateAircrafts()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    unsigned int i;
    for (i = 0; i < vec.size(); ++i) {
        vec.at(i)->valid++;
        if (vec.at(i)->valid >= INVALIDATE) {
            delete vec.at(i);
            vec.erase(vec.begin() + i);
        }
    }
    return;
}

Aircraft* AircraftContainer::getAircraft(unsigned int i)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    if (i < 0 || i >= vec.size()) {
        return nullptr;
    } else {
        return vec.at(i);
    }
}

unsigned int AircraftContainer::getContSize()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return vec.size();
}
