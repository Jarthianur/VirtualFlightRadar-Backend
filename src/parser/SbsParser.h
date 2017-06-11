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

#ifndef SRC_PARSER_SBSPARSER_H_
#define SRC_PARSER_SBSPARSER_H_

#include <cstdint>
#include <string>

#include "Parser.h"
#include "../util/Position.hpp"

namespace parser
{

/**
 * The SbsParser class, implements Parser.
 *
 * This class unpacks SBS sentences into static VFRB::AircraftContainer.
 */
class SbsParser: public Parser
{
public:
    /**
     * Constructor
     */
    SbsParser();
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~SbsParser() noexcept;
    /**
     * Unpack SBS messages into the static VFRB::AircraftContainer.
     *
     * @overload Parser::unpack
     */
    std::int32_t unpack(const std::string& /*cr_msg*/, std::int32_t /*prio*/)
            noexcept override;

private:
    /// Temporary data
    std::string mtId;
    struct util::GPSPosition mtGpsPos;
    //std::int32_t mtTime = 0;
};

}  // namespace parser

#endif /* SRC_PARSER_SBSPARSER_H_ */
