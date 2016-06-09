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
    const auto it = index_map.find(id);
    if (it == index_map.end()) {
        //std::cout << id << " not found" << std::endl;
        return -1;
    } else {
        //std::cout << id << " found at " << it->second << std::endl;
        return it->second;
    }
}

void AircraftContainer::invalidateAircrafts()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    int i = 0, x = 0;
    int size = cont.size();
    bool del = false;

    /*std::cout << "before invalidation" << std::endl;
    for (Aircraft& ac : cont) std::cout << ac.id << " at " << x++ << " : " << ac.valid << " | ";std::cout << std::endl;x=0;
    for (const auto& it : index_map) std::cout << it.first << " at " << it.second << " | ";std::cout << std::endl;*/

    while (i < size && size > 0) {
        try {
            if (++(cont.at(i).valid) >= INVALIDATE) {
                del = true;
                index_map.erase(index_map.find(cont.at(i).id));
                cont.erase(cont.begin() + i);
                size--;
            } else {
                i++;
            }
            if (del && size > 0 && i < size) {
                index_map.at(cont.at(i).id) = i;
            }
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
            std::terminate();
        }
    }
    /*std::cout << "after invalidation" << std::endl;
    for (Aircraft& ac : cont) std::cout << ac.id << " at " << x++ << " : " << ac.valid << " | ";std::cout << std::endl;x=0;
    for (const auto& it : index_map) std::cout << it.first << " at " << it.second << " | ";std::cout << std::endl<< std::endl;*/

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
        index_map.insert({id,cont.size()-1});
        //std::cout << id << " inserted at " << cont.size()-1 << std::endl;
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
        index_map.insert({id,cont.size()-1});
        //std::cout << id << " inserted at " << cont.size()-1 << std::endl;
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
    index_map.clear();
    return;
}
