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

#ifndef SRC_CONFIG_PROPERTYMAP_H_
#define SRC_CONFIG_PROPERTYMAP_H_

#include <string>
#include <unordered_map>

namespace config
{

using keyValueMap = std::unordered_map<std::string, std::string>;
using keyValue = std::pair<std::string, std::string>;

class PropertyMap
{
public:
    PropertyMap();
    virtual ~PropertyMap();
    /**
     * @fn getProperty
     * @brief Get the value under a section for the given key.     *
     * @param cr_section The section of the property
     * @param cr_key     The key to get the value for
     * @param cr_def_val The default value; defaults to empty string
     * @return the value for key in section, if found, else the default value
     */
    const std::string getProperty(const std::string& cr_section,
            const std::string& cr_key, const std::string& cr_def_val = "") const;
    /**
     * @fn getSectionKv
     * @brief Get the key-value-map for a section.
     * @param cr_section The section to get the kv-map for
     * @return the kv-map if found, else an empty one
     */
    const keyValueMap& getSectionKv(
            const std::string& cr_section) const;

    bool addProperty(const std::string& r_section, const keyValue& r_kv = {});

private:
    /// The map of sections with kv-maps
    std::unordered_map<std::string, keyValueMap> mPropertiesMap;
};

} /* namespace config */

#endif /* SRC_CONFIG_PROPERTYMAP_H_ */
