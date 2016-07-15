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

#ifndef PARSER_H_
#define PARSER_H_

#include "AircraftContainer.h"

class Parser
{
public:
    Parser();
    virtual ~Parser() throw();

    /**
     * unpack a msg into Aircraft and insert in Container.
     * returns 0 on success, -1 on failure.
     */
    virtual int unpack(const std::string&, AircraftContainer&) = 0;

};

#endif /* PARSER_H_ */
