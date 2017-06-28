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

#include <string>

#include "../aircraft/Aircraft.hpp"
#include "Parser.hpp"

namespace parser
{

/**
 * @class SbsParser implements Parser
 * @brief Provide unpacking method for SBS sentences.
 * @see Parser.hpp
 * @see ../aircraft/Aircraft.hpp
 */
class SbsParser: public Parser<aircraft::Aircraft>
{
public:
    /**
     * @fn SbsParser
     * @brief Constructor
     */
    SbsParser();
    /**
     * @fn ~SbsParser
     * @brief Destructor
     */
    virtual ~SbsParser() noexcept;
    /**
     * @fn unpack
     * @brief Unpack into Aircraft.
     * @override Parser::unpack
     */
    bool unpack(const std::string& cr_msg, aircraft::Aircraft& r_ac) noexcept override;
};

}  // namespace parser

#endif /* SRC_PARSER_SBSPARSER_H_ */
