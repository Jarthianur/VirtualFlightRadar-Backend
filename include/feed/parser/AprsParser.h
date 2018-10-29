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

#pragma once

#include <cstdint>
#include <string>

#include <boost/regex.hpp>

#include "object/Aircraft.h"
#include "util/defines.h"

#include "Parser.hpp"

/// @namespace feed
namespace feed
{
/// @namespace parser
namespace parser
{
/**
 * @class AprsParser
 * @brief Unpacking APRS sentences.
 * @implements Parser
 */
class AprsParser : public Parser<object::Aircraft>
{
public:
    AprsParser();

    ~AprsParser() noexcept;

    /**
     * @fn unpack
     * @brief Unpack into Aircraft.
     * @see Parser#unpack
     * @return true on success, else false
     */
    bool unpack(const std::string& sentence, object::Aircraft& aircraft) noexcept override;

    /// @var mMaxHeight
    /// The max height filter
    static std::int32_t s_maxHeight;

private:
    /**
     * @fn parsePosition
     * @brief Parse a Position.
     * @param crMatch   The regex match
     * @param rAircraft The target Aircraft
     * @return true on success, else false
     */
    bool parsePosition(const boost::smatch& match, object::Aircraft& aircraft) noexcept;

    /**
     * @fn parseComment
     * @brief Parse the APRS comment.
     * @param crMatch   The regex match
     * @param rAircraft The target Aircraft
     * @return true on success, else false
     */
    bool parseComment(const boost::smatch& match, object::Aircraft& aircraft) noexcept;

    /**
     * @fn parseMovement
     * @brief Parse the Movement information.
     * @param crMatch     The regex match
     * @param crCommMatch The comment regex match
     * @param rAircraft   The target Aircraft
     * @return true on success, else false
     */
    bool parseMovement(const boost::smatch& match, const boost::smatch& comMatch,
                       object::Aircraft& aircraft) noexcept;

    /**
     * @fn parseTimeStamp
     * @brief Parse the TimeStamp information.
     * @param crMatch     The regex match
     * @param rAircraft   The target Aircraft
     * @return true on success, else false
     */
    bool parseTimeStamp(const boost::smatch& match, object::Aircraft& aircraft) noexcept;

    /// @var msAprsRe
    /// Regular expression for APRS protocol
    static const boost::regex s_APRS_RE;

    /// @var msAprsComRe
    /// Regular expression for OGN specific APRS extension
    static const boost::regex s_APRSExtRE;
};
}  // namespace parser
}  // namespace feed
