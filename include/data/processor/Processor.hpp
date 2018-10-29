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

#include <cstdio>
#include <string>

#include "util/math.hpp"

namespace data
{
namespace processor
{
/**
 * @brief Processor base class/interface.
 * @tparam T The type of object to process
 */
template<typename T>
class Processor
{
public:
    /**
     * @brief Constructor
     */
    Processor() = default;

    /**
     * @brief Destructor
     */
    virtual ~Processor() noexcept = default;

    /**
     * @brief Process an object.
     * @param _1 The object of type T
     */
    virtual void process(T& _1) = 0;

protected:
    /**
     * @brief End the processing string with checksum and CRLF.
     */
    inline void finishSentence()
    {
        std::snprintf(m_buffer, sizeof(m_buffer), "%02x\r\n",
                      math::checksum(m_buffer, sizeof(m_buffer)));
        m_processed.append(m_buffer);
    }

    /// The internal buffer for format strings
    char m_buffer[4096] = "";

    mutable std::string m_processed;
};
}  // namespace processor
}  // namespace data
