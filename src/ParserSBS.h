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

#ifndef PARSERSBS_H_
#define PARSERSBS_H_

#include <string>

#include "Parser.h"

class ParserSBS: public Parser
{
public:
    ParserSBS();
    virtual ~ParserSBS() throw ();

    int unpack(const std::string&, AircraftContainer&);

private:
    std::string id;
    int alt = 0, time = 0;
    double lat = 0.0, lon = 0.0;
};

#endif /* PARSERSBS_H_ */
