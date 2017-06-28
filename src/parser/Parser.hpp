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

#ifndef SRC_PARSER_PARSER_HPP_
#define SRC_PARSER_PARSER_HPP_

#include <string>

namespace aircraft
{
class Aircraft;
} /* namespace aircraft */

namespace parser
{

/**
 * The Parser interface.
 *
 * Classes of this type implement the unpack method.
 */
template<typename T>
class Parser
{
public:
    /**
     * Constructor
     */
    inline Parser()
    {
    }
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    inline virtual ~Parser() noexcept
    {
    }
    /**
     * Unpack given string.
     * Where to put unpacked data is handled by implementation,
     * as well as priority pass-through.
     *
     * @param cr_msg the msg to unpack
     * @param prio  the priority to pass
     *
     * @return an error code
     */
    virtual bool unpack(const std::string& cr_msg, T&) noexcept = 0;
};

}  // namespace parser

#endif /* SRC_PARSER_PARSER_HPP_ */
