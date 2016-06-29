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

#include "ParserADSB.h"
#include "Configuration.h"
#include "Math.h"

ParserADSB::ParserADSB()
: Parser()
{
}

ParserADSB::~ParserADSB()
{
}

int ParserADSB::unpack(const std::string& sentence, AircraftContainer& ac_cont)
{
    std::string msg = sentence;
    /*
     * fields:
     * 4 : id
     * 7 : time
     * 11: altitude
     * 14: latitude
     * 15: longitude
     */
    try {
        msg.erase(0,6);
    } catch (std::out_of_range& e) {
        return -1;
    }
    int delim, i = 2;
    while((delim = msg.find(',')) > -1) {
        switch(i) {
            case 4:
                try {
                    id = msg.substr(0,delim);
                } catch (std::out_of_range& e) {
                    return -1;
                }
                break;
            case 7:
                try {
                    time = std::stoi(msg.substr(0,2), nullptr) * 10000;
                    time += std::stoi(msg.substr(3,2), nullptr) * 100;
                    time += std::stoi(msg.substr(6,2), nullptr);
                } catch (std::logic_error& e) {
                    return -1;
                }
                break;
            case 11:
                try {
                    alt = Math::dToI(std::stod(msg.substr(0,delim), nullptr) * Math::feet2m);
                    if (alt > Configuration::filter_maxHeight) return -1;
                } catch (std::logic_error& e) {
                    return -1;
                }
                break;
            case 14:
                try {
                    lat = std::stod(msg.substr(0,delim), nullptr);
                } catch (std::logic_error& e) {
                    return -1;
                }
                break;
            case 15:
                try {
                    lon = std::stod(msg.substr(0,delim), nullptr);
                } catch (std::logic_error& e) {
                    return -1;
                }
                break;
            default:
                break;
        }
        i++;
        try {
            msg.erase(0,delim+1);
        } catch (std::out_of_range& e) {
            return -1;
        }
    }
    ac_cont.insertAircraft(id, lat, lon, alt, time);
    return 0;
}
