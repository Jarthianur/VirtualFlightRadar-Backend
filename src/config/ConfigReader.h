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

#include <istream>

#include "Properties.h"

/// @namespace config
namespace config
{
/**
 * @class ConfigReader
 * @brief Read a config in INI format.
 *
 * Provide methods to read any input, in INI format, and store those properties.
 */
class ConfigReader
{
public:
    ConfigReader(std::istream& stream);

    ~ConfigReader() noexcept = default;

    /**
     * @fn read
     * @brief Read the given stream and store valid properties in a map.
     * @param rStream The stream to read
     * @param rMap    The map to store the properties in
     */
    Properties read();

private:
    std::istream& m_stream;
};

}  // namespace config
