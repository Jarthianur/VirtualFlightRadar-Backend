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

#ifndef SRC_CONFIG_CONFIGREADER_H_
#define SRC_CONFIG_CONFIGREADER_H_

#include <boost/regex.hpp>
#include <istream>

namespace config
{

class PropertyMap;

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
     * @param r_stream The stream to read
     * @param r_map  The map to store the properties in
     */
    void read(std::istream& r_stream, PropertyMap& r_map);

private:
    /// The regular expression for 'key = value' lines
    const boost::regex mConfRe;
};

}  // namespace config

#endif /* SRC_CONFIG_CONFIGREADER_H_ */
