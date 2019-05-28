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

#include "object/GpsPosition.h"

#include "Parser.hpp"

namespace feed
{
namespace parser
{
/**
 * @brief Implement Parser for GPS NMEA sentences.
 */
class GpsParser : public Parser<object::GpsPosition>
{
public:
    GpsParser();
    ~GpsParser() noexcept override = default;

    /**
     * @brief Unpack into GpsPosition.
     * @param sentence The string to unpack
     * @param position The position to unpack into
     * @return true on success, else false
     */
    bool unpack(const std::string& sentence, object::GpsPosition& position) noexcept override;

private:
    /**
     * @brief Parse a Position.
     * @param match    The regex match
     * @param position The target position
     * @return true on success, else false
     */
    bool parsePosition(const boost::smatch& match, object::GpsPosition& position);

    /// Regular expression to parse GGA
    static const boost::regex s_GPGGA_RE;
};
}  // namespace parser
}  // namespace feed
