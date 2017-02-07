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

#include "AircraftContainer.h"

#include <cstdint>
#include <exception>
#include <iterator>
#include <utility>

#include "../io/logger/Logger.h"
#include "../util/Configuration.h"

AircraftContainer::AircraftContainer()
        : proc(Configuration::base_latitude, Configuration::base_longitude,
               Configuration::base_altitude, Configuration::base_geoid)
{
}

AircraftContainer::~AircraftContainer()
{
}

ssize_t AircraftContainer::find(std::string& id)
{
    const auto it = index_map.find(id);
    if (it == index_map.end())
    {
        return AC_NOT_FOUND;
    }
    else
    {
        return it->second;
    }
}

std::string AircraftContainer::processAircrafts()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    std::string dest_str;
    size_t index = 0;
    bool del = false;
    auto it = cont.begin();

    while (it != cont.end())
    {
        try
        {
            if (++(it->valid) >= AC_INVALIDATE)
            {
                del = true;
                index_map.erase(index_map.find(it->id));
                cont.erase(it);
            }
            else
            {
                dest_str += proc.process(*it);
                ++it;
                ++index;
            }
            if (del && it != cont.end())
            {
                index_map.at(it->id) = index;
            }
        }
        catch (std::exception& e)
        {
            Logger::warn("(AircraftContainer) processAircrafts: ", e.what());
        }
    }
    return dest_str;
}

void AircraftContainer::insertAircraft(std::string& id, double lat, double lon,
        int32_t alt)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    ssize_t i;
    if ((i = find(id)) == AC_NOT_FOUND)
    {
        Aircraft ac(id, lat, lon, alt);
        ac.qne = true;
        cont.push_back(ac);
        index_map.insert(
        { id, cont.size() - 1 });
    }
    else
    {
        Aircraft& ac = cont.at(i);
        ac.valid = 0;
        ac.latitude = lat;
        ac.longitude = lon;
        ac.altitude = alt;
        ac.qne = true;
    }
}

void AircraftContainer::insertAircraft(std::string& id, double lat, double lon,
        int32_t alt, double gnd_spd, uint32_t id_t, int32_t ac_t, double climb_r,
        double turn_r, double heading)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    int32_t act;
    ssize_t i;
    if (heading == A_VALUE_NA || gnd_spd == A_VALUE_NA || climb_r == A_VALUE_NA)
    {
        act = A_MIN_DATA;
    }
    else
    {
        act = ac_t;
    }
    if ((i = find(id)) == -1)
    {
        Aircraft ac(id, lat, lon, alt, gnd_spd, id_t, act, climb_r, turn_r, heading);
        ac.qne = false;
        cont.push_back(ac);
        index_map.insert(
        { id, cont.size() - 1 });
    }
    else
    {
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
}
