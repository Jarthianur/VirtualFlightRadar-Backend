/*
 * AircraftContainer.cpp
 *
 *  Created on: 01.05.2016
 *      Author: lula
 */

#include "AircraftContainer.h"
#include <iostream>

AircraftContainer::AircraftContainer()
{
}

AircraftContainer::~AircraftContainer()
{
    clear();
}

int AircraftContainer::find(std::string& id)
{
    unsigned int i;
    for (i = 0; i < cont.size(); ++i) {
        if (cont.at(i).id.compare(id) == 0) {
            return i;
        }
    }
    return -1;
}

void AircraftContainer::invalidateAircrafts()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    int i;
    int size = cont.size();
    for (i = 0; i < size; ++i) {
        if (++(cont.at(i).valid) >= INVALIDATE) {
            cont.erase(cont.begin() + i);
            size--;
        }
    }
    return;
}

Aircraft& AircraftContainer::getAircraft(unsigned int i)
{
    return std::ref(cont.at(i));
}

unsigned int AircraftContainer::getContSize()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return cont.size();
}

void AircraftContainer::insertAircraft(long double lat, long double lon,
        int alt, std::string& id)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    int i;
    if ((i = find(id)) == -1) {
        Aircraft ac(id, lat, lon, alt);
        ac.aircraft_type = -1;
        cont.push_back(ac);
    } else {
        Aircraft& ac = getAircraft(i);
        ac.latitude = lat;
        ac.longitude = lon;
        ac.altitude = alt;
        ac.aircraft_type = -1;
        ac.valid = 0;
    }
    return;
}

void AircraftContainer::insertAircraft(long double lat, long double lon,
        int alt, std::string& id, int addr_t, int ac_t, int climb_r,
        int gnd_spd, float heading)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    int i;
    if ((i = find(id)) == -1) {
        Aircraft ac(id, lat, lon, alt, heading, gnd_spd, addr_t, ac_t, climb_r);
        cont.push_back(ac);
    } else {
        Aircraft& ac = getAircraft(i);
        ac.latitude = lat;
        ac.longitude = lon;
        ac.altitude = alt;
        ac.aircraft_type = ac_t;
        ac.climb_rate = climb_r;
        ac.gnd_speed = gnd_spd;
        ac.heading = heading;
        ac.addr_type = addr_t;
        ac.valid = 0;
    }
    return;
}

void AircraftContainer::clear()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    cont.clear();
    return;
}
