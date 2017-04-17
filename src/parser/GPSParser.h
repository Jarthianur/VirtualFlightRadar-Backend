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

#ifndef SRC_PARSER_GPSPARSER_H_
#define SRC_PARSER_GPSPARSER_H_

#include <boost/regex.hpp>
#include "Parser.h"

struct GPSPosition;

class GPSParser: public Parser
{
public:
    GPSParser();
    virtual ~GPSParser() noexcept;

    std::int32_t unpack(const std::string& /*msg*/, Priority /*prio*/) noexcept override;

private:
    //regex
    const boost::regex mGpggaRE;
    //tmps
    struct GPSPosition mtGPSpos;
};

#endif /* SRC_PARSER_GPSPARSER_H_ */
