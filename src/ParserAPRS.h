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

#ifndef PARSERAPRS_H_
#define PARSERAPRS_H_

#include "Parser.h"
#include <regex>

class ParserAPRS: public Parser
{
public:
    ParserAPRS();
    virtual ~ParserAPRS() throw();

    int unpack(const std::string&, AircraftContainer&);

private:
    //regex
    const std::regex aprs_re;
    const std::regex comm_re;
    // temps
    std::string id;
    int id_t, ac_t, alt, time;
    double lat, lon, turn_r, climb_r, gnd_spd, heading;
};

#endif /* PARSERAPRS_H_ */
