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

#include "object/Aircraft.h"
#include "util/defines.h"

#include "Parser.hpp"

namespace feed
{
namespace parser
{
/**
 * @brief Implement Parser for SBS sentences.
 */
class SbsParser : public Parser<object::Aircraft>
{
public:
    /**
     * @brief Constructor
     */
    SbsParser();

    /**
     * @brief Destructor
     */
    ~SbsParser() noexcept = default;

    /**
     * @brief Unpack into Aircraft.
     * @param sentence The string to unpack
     * @param aircraft The Aircraft to unpack into
     */
    bool unpack(const std::string& sentence, object::Aircraft& aircraft) noexcept override;

    /// The max height filter
    static std::int32_t s_maxHeight;

private:
    /**
     * @brief Parse a field in SBS and set respective values.
     * @param fieldNr  The field number
     * @param field    The string in that field
     * @param position The target position
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    bool parseField(std::uint32_t fieldNr, const std::string& field, object::Position& position,
                    object::Aircraft& aircraft) noexcept;
};
}  // namespace parser
}  // namespace feed
