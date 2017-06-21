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

#include <cstdint>
#include <string>

namespace util
{

struct SensInfo {

};

/**
 * The Parser interface.
 *
 * Classes of this type implement the unpack method.
 */
class Parser
{
public:
    /**
     * Constructor
     */
    Parser();
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~Parser() noexcept;

    static aircraft::Aircraft parseAprs(const std::string& msg);
    static aircraft::Aircraft parseSbs(const std::string& msg);
    static ExtGPSPosition parseGpsNmea(const std::string& msg);
    static SensInfo parseSensNmea(const std::string& msg);
};

}  // namespace util

#endif /* SRC_PARSER_PARSER_H_ */
