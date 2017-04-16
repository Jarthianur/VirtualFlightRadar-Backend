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

#ifndef SRC_PARSER_APRSPARSER_H_
#define SRC_PARSER_APRSPARSER_H_

#include <boost/regex.hpp>
#include <cstdint>
#include <string>

#include "Parser.h"

#define APRS_REGEX_ERR -3

class APRSParser: public Parser
{
public:
    APRSParser();
    virtual ~APRSParser() noexcept;

    std::int32_t unpack(const std::string& /*msg*/, Priority /*prio*/) noexcept override;

private:
    //regex
    const boost::regex mAprsRE;
    const boost::regex mCommRE;
    // temps
    std::string mtID;
    std::int32_t mtIDtype = 0, mtAcType = 0, mtAlt = 0, mtTime = 0;
    double mtLat = 0.0, mtLong = 0.0, mtTurnRate = 0.0, mtClimbRate = 0.0, mtGndSpeed = 0.0,
            mtHeading = 0.0;
    bool mtFullInfo = true;
};

#endif /* SRC_PARSER_APRSPARSER_H_ */
