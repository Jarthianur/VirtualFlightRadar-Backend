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
    clear();
}

int AircraftContainer::find(std::string& id)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    unsigned int i;
    for (i = 0; i < cont.size(); ++i) {
        if (cont.at(i)->id.compare(id) == 0) {
            return i;
        }
    }
    return -1;
}

void AircraftContainer::pushAircraft(Aircraft* ac)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    cont.push_back(ac);
    return;
}

void AircraftContainer::invalidateAircrafts()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    unsigned int i;
    for (i = 0; i < cont.size(); ++i) {
        cont.at(i)->valid++;
        if (cont.at(i)->valid >= INVALIDATE) {
            delete cont.at(i);
            cont.erase(cont.begin() + i);
        }
    }
    return;
}

Aircraft* AircraftContainer::getAircraft(unsigned int i)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    if (i < 0 || i >= cont.size()) {
        return nullptr;
    } else {
        return cont.at(i);
    }
}

unsigned int AircraftContainer::getContSize()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return cont.size();
}

void AircraftContainer::clear()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    for (Aircraft* ac : cont) {
        delete ac;
    }
    cont.clear();
    return;
}
