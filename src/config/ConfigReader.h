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

#include <cstddef>
#include <istream>
#include <string>
#include <boost/optional.hpp>
#include <boost/regex.hpp>

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
    ConfigReader();

    ~ConfigReader() noexcept;

    /**
     * @fn read
     * @brief Read the given stream and store valid properties in a map.
     * @param rStream The stream to read
     * @param rMap    The map to store the properties in
     */
    void read(std::istream& stream, Properties& properties);

private:
    /**
     * @fn parseSection
     * @brief Parse a section header.
     *
     * A section begins with '[name]', which is the header.
     *
     * @param crLine The string to parse
     * @return an optional section name
     */
    boost::optional<std::string> parseSection(const std::string& line);

    /**
     * @fn parseKeyValue
     * @brief Parse a key-value pair.
     * @see mConfRe
     * @param crLine The string to parse
     * @return an optional KeyValue
     */
    boost::optional<KeyValue> parseProperty(const std::string& line);

    /// @var mConfRe
    /// Regular expression for 'key = value'
    const static boost::regex s_keyValueRE;
};

}  // namespace config
