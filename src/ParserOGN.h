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

#ifndef PARSEROGN_H_
#define PARSEROGN_H_

#include "Parser.h"
#include <vector>
#include <regex>

class ParserOGN: public Parser
{
public:
    ParserOGN();
    virtual ~ParserOGN() throw();

    int unpack(const std::string&, AircraftContainer&);

private:
    void splitToTokens(const std::string&);
    //regex
    const std::regex aprs_re;
    const std::regex addr_re;
    const std::regex climb_re;
    //comment tokens
    std::vector<std::string> tokens;
    std::string comment;
    std::string id;
    int addr_t, ac_t, climb_r, alt, gnd_spd;
    long double lat, lon;
    float heading;
};

#endif /* PARSEROGN_H_ */
