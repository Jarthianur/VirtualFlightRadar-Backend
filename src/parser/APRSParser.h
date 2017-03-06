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

#ifndef APRSPARSER_H_
#define APRSPARSER_H_

#include <boost/regex.hpp>
#include <cstdint>
#include <string>

#include "Parser.h"

#define APRS_REGEX_ERR -3

class APRSParser: public Parser
{
public:
    APRSParser();
    virtual ~APRSParser() throw ();

    std::int32_t unpack(const std::string&);

private:
    //regex
    const boost::regex aprs_re;
    const boost::regex comm_re;
    // temps
    std::string id;
    std::int32_t id_t = 0, ac_t = 0, alt = 0, time = 0;
    double lat = 0.0, lon = 0.0, turn_r = 0.0, climb_r = 0.0, gnd_spd = 0.0,
            heading = 0.0;
};

#endif /* APRSPARSER_H_ */
