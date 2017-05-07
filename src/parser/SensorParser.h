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

#include <cstdint>
#include <cstddef>
#include <string>

#include "Parser.h"

namespace parser
{

/**
 * The SensorParser class, implements Parser.
 *
 * This class unpacks NMEA sentences, as they are MDA and MWV, into static VFRB::SensorData.
 */
class SensorParser: public Parser
{
public:
    /**
     * Constructor
     */
    SensorParser();
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~SensorParser() noexcept;
    /**
     * Unpack NMEA messages into the static VFRB::SensorData.
     *
     * @overload Parser::unpack
     */
    std::int32_t unpack(const std::string& /*cr_msg*/, util::Priority /*prio*/)
            noexcept override;

private:
    /// Temporary data
    std::size_t mtB = 0, mtS = 0, mtSubLen = 0, mtNumIdx = 0;
    double mtPress = 0.0;
};

}  // namespace parser

#endif /* SRC_PARSER_SENSORPARSER_H_ */
