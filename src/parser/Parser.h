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

#include "../util/Priority.h"

namespace parser
{

/// Parser -- unpack error codes
#define MSG_UNPACK_SUC 0
#define MSG_UNPACK_ERR -1
#define MSG_UNPACK_IGN -2

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
    /**
     * Unpack given string.
     * Where to put unpacked data is handled by implementation,
     * as well as Priority pass-through.
     *
     * @param cr_msg the msg to unpack
     * @param prio  the Priority to pass
     *
     * @return an error code
     */
    virtual std::int32_t unpack(const std::string& /*cr_msg*/,
                                util::Priority /*prio*/) noexcept = 0;
};

}  // namespace parser

#endif /* SRC_PARSER_PARSER_H_ */
