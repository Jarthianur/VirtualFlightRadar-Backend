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
#include "../util/Position.hpp"

namespace parser
{

/**
 * The APRSParser class, implements Parser.
 *
 * This class unpacks APRS strings into static AircraftContainer.
 */
class APRSParser: public Parser
{
public:
    /**
     * Constructor
     */
    APRSParser();
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~APRSParser() noexcept;
    /**
     * Unpack APRS messages into the static VFRB::AircraftContainer.
     *
     * @overload Parser::unpack
     */
    std::int32_t unpack(const std::string& /*cr_msg*/, util::Priority /*prio*/)
            noexcept override;

private:
    /// Regular expression for APRS protocol
    const boost::regex mAprsRE;
    /// Regular expression for OGN specific APRS extension
    const boost::regex mCommRE;
    /// Temporary information
    std::string mtID;
    struct util::GPSPosition mtGPSpos;
    std::int32_t mtIDtype = 0, mtAcType = 0, mtTime = 0;
    double mtTurnRate = 0.0, mtClimbRate = 0.0, mtGndSpeed = 0.0, mtHeading =
            0.0;
};

}  // namespace parser

#endif /* SRC_PARSER_APRSPARSER_H_ */
