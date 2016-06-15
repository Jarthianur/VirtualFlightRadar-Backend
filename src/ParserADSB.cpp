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
     * 11: altitude
     * 14: latitude
     * 15: longitude
     */
    msg.erase(0,6);
    int delim, i = 2;
    while((delim = msg.find(',')) > -1) {
        switch(i) {
            case 4:
                id = msg.substr(0,delim);
                break;
            case 11:
                if (msg.substr(0,delim).length() > 0) {
                    alt = std::stoi(msg.substr(0,delim), nullptr);
                    if (alt > Configuration::filter_maxHeight) return -1;
                } else {
                    return -1;
                }
                break;
            case 14:
                if (msg.substr(0,delim).length() > 0)
                    lat = std::stold(msg.substr(0,delim), nullptr);
                else {
                    return -1;
                }
                break;
            case 15:
                if (msg.substr(0,delim).length() > 0)
                    lon = std::stold(msg.substr(0,delim), nullptr);
                else {
                    return -1;
                }
                break;
            default:
                break;
        }
        i++;
        msg.erase(0,delim+1);
    }
    ac_cont.insertAircraft(lat, lon, alt, id);
    return 0;
}
