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
#include <string>
#include <unordered_map>
#include <istream>

namespace config
{

/**
 * The ConfigReader class.
 *
 * This class reads a configuration file in INI format
 * and stores read parameters in a map.
 */
class ConfigReader
{
public:
    /**
     * Constructor
     */
    ConfigReader();
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~ConfigReader() noexcept;
    /**
     * Read the given file.
     *
     * @param cr_file the file stream to read
     */
    void read(std::istream& r_file);
    /**
     * Get a property from all stored properties.
     * Return the given default value, if the property was not found.
     *
     * @param cr_section the section of the property
     * @param cr_key     the key to get the value for
     * @param cr_def_val the default value; defaults to empty string
     *
     * @return the value for key in section, if found, else the default value
     */
    const std::string getProperty(const std::string& cr_section,
            const std::string& cr_key, const std::string& cr_def_val = "") const;
    /**
     * Get the key-value-map for a section.
     * If the section is not found the kv-map is empty.
     *
     * @param cr_section the section to get the kv-map for
     *
     * @return the kv-map for the section
     */
    const std::unordered_map<std::string, std::string>& getSectionKV(
            const std::string& cr_section) const;

private:
    /// The map of sections with kv-maps
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> mConfig;
    /// The regular expression for 'key = value' lines
    const boost::regex mConfRE;
};

}  // namespace config

#endif /* SRC_CONFIG_CONFIGREADER_H_ */
