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

#ifndef SRC_PARSER_SENSORPARSER_H_
#define SRC_PARSER_SENSORPARSER_H_

#include <string>

#include "../util/SensorInfo.h"
#include "Parser.hpp"

namespace parser
{

/**
 * @class SensorParser implements Parser
 * @brief Provide unpacking method for NMEA sentences from sensors.
 * @see Parser.hpp
 * @see ../util/SensorInfo.h
 */
class SensorParser: public Parser<struct util::SensorInfo>
{
public:
    /**
     * @fn SensorParser
     * @brief Constructor
     */
    SensorParser();
    /**
     * @fn ~SensorParser
     * @brief Destructor
     */
    virtual ~SensorParser() noexcept;
    /**
     * @fn unpack
     * @brief Unpack into SensorInfo.
     * @override Parser::unpack
     */
    bool unpack(const std::string& cr_msg, struct util::SensorInfo& r_info)
            noexcept override;
};

}  // namespace parser

#endif /* SRC_PARSER_SENSORPARSER_H_ */