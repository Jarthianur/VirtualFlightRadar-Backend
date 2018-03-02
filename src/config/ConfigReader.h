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

#include <boost/regex.hpp>
#include <boost/tuple/tuple.hpp>
#include <cstddef>
#include <istream>
#include <string>

#include "PropertyMap.h"

/// @namespace config
namespace config
{
/**
 * @class ConfigReader
 * @brief Provide functions to read properties from an input stream into a map.
 */
class ConfigReader
{
public:
    /**
     * @fn ConfigReader
     * @brief Constructor
     */
    ConfigReader();

    /**
     * @fn ~ConfigReader
     * @brief Destructor
     */
    virtual ~ConfigReader() noexcept;

    /**
     * @fn read
     * @brief Read the given stream and store valid properties in a map.
     * @param rStream The stream to read
     * @param rMap  The map to store the properties in
     */
    void read(std::istream& rStream, PropertyMap& rMap);

private:
    /**
     * @fn parseSection
     * @brief Parse a section.
     * @param crLine The string to parse
     * @return a tuple with the section and a valid flag
     */
    boost::tuple<bool, std::string> parseSection(const std::string& crLine);

    /**
     * @fn parseKeyValue
     * @brief Parse a key-value pair.
     * @see mConfRe
     * @param crLine The string to parse
     * @return a tuple with the KeyValue and a valid flag
     */
    boost::tuple<bool, KeyValue> parseKeyValue(const std::string& crLine);

    /**
     * @fn addKeyValue
     * @brief Add a key-value pair to the properties.
     * @param crSection  The section name
     * @param crKeyValue The key-value pair
     * @param vLineNr    The corresponding line number
     * @param rMap       The properties
     */
    void addKeyValue(const std::string& crSection, const KeyValue& crKeyValue,
                     std::size_t vLineNr, PropertyMap& rMap);

    /**
     * @fn addSection
     * @brief Add a section to the properties.
     * @param crSection The section name
     * @param vLineNr   The corresponding line number
     * @param rMap      The properties
     */
    void addSection(const std::string& crSection, std::size_t vLineNr, PropertyMap& rMap);

    /// @var mConfRe
    /// Regular expression for 'key = value'
    const boost::regex mConfRe;
};

}  // namespace config
