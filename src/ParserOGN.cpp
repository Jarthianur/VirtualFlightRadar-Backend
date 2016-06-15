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

#include "ParserOGN.h"
#include <cstdio>
#include "AircraftContainer.h"
#include "Math.h"
#include "Configuration.h"

ParserOGN::ParserOGN()
: Parser(),
  aprs_re("^(?:\\S+)>APRS,\\S+(?:,\\S+)?:/(?:\\d{6}h)(\\d{4}\\.\\d{2})(N|S)\\S?(\\d{5}\\.\\d{2})(E|W)\\S?((\\d{3})/(\\d{3}))?/A=(\\d{6})\\s?([^]*?)$"),
  comm_re("^[^]*id(\\S{2})(\\S{6})[^]*([\\-|+]\\d+)fpm[^]*$")
{
}

ParserOGN::~ParserOGN()
{
}

int ParserOGN::unpack(const std::string& sentence, AircraftContainer& ac_cont)
{
    if (sentence.at(0) == '#') {
        return -1;
    }
    try {
        std::smatch match;
        if (std::regex_match(sentence, match, aprs_re)) {
            //altitude
            if (match.str(8).size() > 0) alt = Math::ldToI(std::stold(match.str(8)));
            if (alt > Configuration::filter_maxHeight) return -1;

            //comment
            // climbrate / address / id / type
            if (match.str(9).size() > 0) {
                std::smatch comm_match;
                if (std::regex_match(match.str(9), comm_match, comm_re)) {
                    id = comm_match.str(2);
                    addr_t = std::stoi(comm_match.str(1), nullptr, 16) & 0x03;
                    ac_t = (std::stoi(comm_match.str(1), nullptr, 16) & 0x7C) >> 2;
                    climb_r = Math::ldToI(std::stold(comm_match.str(3)) * Math::fpm2ms);
                } else return -1;
            } else return -1;

            //latitude
            lat = Math::dmsToDeg(std::stold(match.str(1)) / 100.0L);
            if (match.str(2).compare("S") == 0) lat = -lat;
            //longitue
            lon = Math::dmsToDeg(std::stold(match.str(3)) / 100.0L);
            if (match.str(4).compare("W") == 0) lon = -lon;

            //track/gnd_speed
            if (match.str(5).size() > 0) {
                heading = std::stoi(match.str(6));
                gnd_spd = Math::ldToI(std::stold(match.str(7)) * Math::kts2kmh);
            } else {
                heading = 0;
                gnd_spd = 0;
            }

            ac_cont.insertAircraft(lat, lon, alt, id, addr_t, ac_t, climb_r, gnd_spd, heading);
        } else {
            return -1;
        }
    } catch (std::regex_error& e) {
        printf("ERROR %d\n", e.code());
        return -1;
    }
    return 0;
}
