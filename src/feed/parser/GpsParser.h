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

#pragma once

#include <string>
#include <boost/regex.hpp>

#include "../../object/GpsPosition.h"
#include "Parser.hpp"

/// @namespace feed
namespace feed
{
/// @namespace parser
namespace parser
{
/**
 * @class GpsParser
 * @brief Unpacking NMEA sentences from GPSD.
 * @implements Parser
 */
class GpsParser : public Parser<object::GpsPosition>
{
public:
    /**
     * @fn GpsParser
     * @brief Constructor
     */
    GpsParser();

    /**
     * @fn ~GpsParser
     * @brief Destructor
     */
    virtual ~GpsParser() noexcept;

    /**
     * @fn unpack
     * @brief Unpack into GpsPosition.
     * @see Parser#unpack
     * @param crStr     The string to unpack
     * @param rPosition The target position
     * @return true on success, else false
     */
    bool unpack(const std::string& crStr, object::GpsPosition& rPosition) noexcept override;

private:
    /**
     * @fn parsePosition
     * @brief Parse a Position.
     * @param crMatch   The regex match
     * @param rPosition The target position
     * @return true on success, else false
     */
    bool parsePosition(const boost::smatch& crMatch, object::GpsPosition& rPosition);

    /// @var msGpggaRe
    /// Regular expression to parse GGA
    static const boost::regex msGpggaRe;
};
}  // namespace parser
}  // namespace feed
