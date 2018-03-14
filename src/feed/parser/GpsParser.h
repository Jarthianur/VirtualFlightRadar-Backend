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

#include <boost/regex.hpp>
#include <string>

#include "../../data/object/Position.h"
#include "Parser.hpp"

namespace feed
{
namespace parser
{
/**
 * @class GpsParser implements Parser
 * @brief Provide unpacking method for NMEA sentences from GPS sensors.
 * @see Parser.hpp
 * @see ../util/Position.h
 */
class GpsParser : public Parser<data::object::ExtGpsPosition>
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
     * @brief Unpack into ExtGpsPosition.
     * @override Parser::unpack
     */
    bool unpack(const std::string& cr_msg,
                data::object::ExtGpsPosition& r_pos) noexcept override;

private:
    // cppcheck-suppress unusedPrivateFunction
    bool parsePosition(const boost::smatch& crMatch, data::object::ExtGpsPosition& rPosition);

    /// Regular expression to parse GGA
    static const boost::regex msGpggaRe;
};
}
}  // namespace parser
