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

#include "AircraftContainer.h"
#include "Configuration.h"
#include <iostream>

AircraftContainer::AircraftContainer()
: proc(Configuration::base_latitude, Configuration::base_longitude, Configuration::base_altitude, Configuration::base_geoid)
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
        return -1;
    } else {
        return it->second;
    }
}

void AircraftContainer::invalidateAircrafts()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    int i = 0;
    int size = cont.size();
    bool del = false;

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
        }
    }
    return;
}

void AircraftContainer::processAircraft(unsigned int i, std::string& dest_str)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    if (i >= cont.size()) return;
    dest_str = proc.process(std::ref(cont.at(i)));
    return;
}

unsigned int AircraftContainer::getContSize()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return cont.size();
}

void AircraftContainer::insertAircraft(std::string& id, double lat, double lon, int alt)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    int i;
    if ((i = find(id)) == -1) {
        Aircraft ac(id, lat, lon, alt);
        ac.qne = true;
        cont.push_back(ac);
        index_map.insert({id,cont.size()-1});
    } else {
        Aircraft& ac = cont.at(i);
        ac.valid = 0;
        ac.latitude = lat;
        ac.longitude = lon;
        ac.altitude = alt;
        ac.qne = true;
    }
    return;
}

void AircraftContainer::insertAircraft(std::string& id, double lat,
        double lon, int alt, double gnd_spd, unsigned int id_t,
        int ac_t, double climb_r, double turn_r, double heading)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    int i, act;
    if (heading == VALUE_NA ||
            gnd_spd == VALUE_NA ||
            climb_r == VALUE_NA) {
        act = MIN_DATA;
    } else {
        act = ac_t;
    }
    if ((i = find(id)) == -1) {
        Aircraft ac(id, lat, lon, alt, gnd_spd, id_t, act, climb_r, turn_r, heading);
        ac.qne = false;
        cont.push_back(ac);
        index_map.insert({id,cont.size()-1});
    } else {
        Aircraft& ac = cont.at(i);
        ac.aircraft_type = act;
        ac.gnd_speed = gnd_spd;
        ac.id_type = id_t;
        ac.valid = 0;
        ac.latitude = lat;
        ac.longitude = lon;
        ac.altitude = alt;
        ac.climb_rate = climb_r;
        ac.turn_rate = turn_r;
        ac.heading = heading;
        ac.qne = false;
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
