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

#ifndef SRC_PARSER_GPSPARSER_H_
#define SRC_PARSER_GPSPARSER_H_

#include <cstdint>
#include <string>
#include <boost/regex.hpp>

#include "../util/Position.hpp"
#include "Parser.hpp"

namespace parser
{

/**
 * The GpsParser class, implememnts Parser.
 *
 * This class unpacks NMEA GPS sentences, as they are GGA (and RMC), into static GPSData container.
 */
class GpsParser: public Parser<struct util::ExtGpsPosition>
{
public:
    /**
     * Constructor
     */
    GpsParser();
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~GpsParser() noexcept;
    /**
     * Unpack GGA messages into the static VFRB::GPSData.
     *
     * @overload Parser::unpack
     */
    bool unpack(const std::string& cr_msg, struct util::ExtGpsPosition& r_pos)
            noexcept override;

private:
    /// Regular expression to parse GGA
    static const boost::regex msGpggaRe;
};

}  // namespace parser

#endif /* SRC_PARSER_GPSPARSER_H_ */
