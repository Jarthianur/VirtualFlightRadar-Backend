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
     *
     * @param r_fname the file to read
     */
    ConfigReader(const std::string& /*r_fname*/);
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~ConfigReader() noexcept;
    /**
     * Read the given file.
     *
     * @exceptsafe strong
     */
    void read() noexcept;
    /**
     * Get a property from all stored properties.
     * Return the given default value, if the property was not found.
     *
     * @param r_section the section of the property
     * @param r_key     the key to get the value for
     * @param r_def_val the default value; defaults to empty string
     *
     * @return the value for key in section, if found, else the default value
     */
    const std::string getProperty(const std::string& /*r_section*/,
                                  const std::string& /*r_key*/,
                                  const std::string& /*r_def_val*/= "") const;
    /**
     * Get the key-value-map for a section.
     * If the section is not found the kv-map is empty.
     *
     * @param r_section the section to get the kv-map for
     *
     * @return the kv-map for the section
     */
    const std::unordered_map<std::string, std::string>& getSectionKV(
            const std::string& /*r_section*/) const;

private:
    /// The filename
    const std::string mFile;
    /// The map of sections with kv-maps
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> mConfig;
    /// The regular expression for 'key = value' lines
    const boost::regex mConfRE;
};

#endif /* SRC_CONFIG_CONFIGREADER_H_ */
