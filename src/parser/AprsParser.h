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

#include "../aircraft/Aircraft.hpp"
#include "Parser.hpp"

namespace parser
{

/**
 * The AprsParser class, implements Parser.
 *
 * This class unpacks APRS strings into static AircraftContainer.
 */
class AprsParser: public Parser<aircraft::Aircraft>
{
public:
    /**
     * Constructor
     */
    AprsParser();
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~AprsParser() noexcept;
    /**
     * Unpack APRS messages into the static VFRB::AircraftContainer.
     *
     * @overload Parser::unpack
     */
    bool unpack(const std::string& cr_msg, aircraft::Aircraft& r_ac) noexcept override;

private:
    /// Regular expression for APRS protocol
    static const boost::regex msAprsRe;
    /// Regular expression for OGN specific APRS extension
    static const boost::regex msAprsComRe;
};

}  // namespace parser

#endif /* SRC_PARSER_APRSPARSER_H_ */
