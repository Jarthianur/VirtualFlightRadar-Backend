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

class SBSParser: public Parser
{
public:
    SBSParser();
    virtual ~SBSParser() noexcept;

    std::int32_t unpack(const std::string& /*msg*/) noexcept override;

private:
    std::string mtID;
    std::int32_t mtAlt = 0, mtTime = 0;
    double mtLat = 0.0, mtLong = 0.0;
};

#endif /* SRC_PARSER_SBSPARSER_H_ */
