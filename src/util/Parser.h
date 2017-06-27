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

#ifndef SRC_PARSER_PARSER_H_
#define SRC_PARSER_PARSER_H_

#include <boost/optional.hpp>
#include <boost/regex.hpp>
#include <string>

#include "../aircraft/Aircraft.h"
#include "Position.hpp"
#include "SensorInfo.h"

namespace aircraft
{
class Aircraft;
}

namespace util
{

/**
 * @class Parser
 * @brief Collection of static methods to parse messages into corresponding objects.
 */
class Parser
{
public:
    /**
     * @fn Parser
     * @brief Constructor
     */
    Parser();
    /**
     * @fn ~Parser
     * @brief Destructor
     */
    virtual ~Parser() noexcept;
    /**
     * @fn parseAprs
     * @brief Parse an APRS sentence into an Aircraft.
     * @see ../aircraft/Aircraft.h
     * @param cr_msg The APRS sentence
     * @return the Aircraft if valid, else boost::none
     * @throws std::logic_error if the sentence is invalid
     */
    static boost::optional<aircraft::Aircraft> parseAprs(const std::string& cr_msg);
    /**
     * @fn parseSbs
     * @brief Parse an SBS sentence into an Aircraft.
     * @see ../aircraft/Aircraft.h
     * @param cr_msg The SBS sentence
     * @return the Aircraft if valid, else boost::none
     * @throws std::logic_error if the sentence is invalid
     */
    static boost::optional<aircraft::Aircraft> parseSbs(const std::string& cr_msg);
    /**
     * @fn parseGgaNmea
     * @brief Parse an NMEA GGA sentence into an ExtGpsPosition.
     * @see Position.hpp
     * @param cr_msg The GGA sentence
     * @return the ExtGpsPosition if valid, else boost::none
     * @throws std::logic_error if the sentence is invalid
     */
    static boost::optional<struct ExtGpsPosition> parseGgaNmea(const std::string& cr_msg);
    /**
     * @fn parseSensNmea
     * @brief Parse an NMEA MDA/MWV sentence into a SensorInfo.
     * @see SensorInfo.h
     * @param cr_msg The NMEA sentence
     * @return the SensorInfo if valid, else boost::none
     * @throws std::logic_error if the sentence is invalid
     */
    static boost::optional<struct SensorInfo> parseSensNmea(const std::string& cr_msg);

private:
    /// Regular expression for APRS protocol
    static const boost::regex aprsRe;
    /// Regular expression for OGN specific APRS extension
    static const boost::regex aprsComRe;
    /// Regular expression for GGA sentence
    static const boost::regex gpggaRe;
};

}  // namespace util

#endif /* SRC_PARSER_PARSER_H_ */
