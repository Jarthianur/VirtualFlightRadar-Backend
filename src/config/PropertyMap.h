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
#include <utility>

namespace config
{

/// Typedef for a map of key-value pairs
using keyValueMap = std::unordered_map<std::string, std::string>;
/// Typedef for a key-value pair
using keyValue = std::pair<std::string, std::string>;

/**
 * @class PropertyMap
 * @brief Store key-value pairs for sections.
 */
class PropertyMap
{
public:
    /**
     * @fn PropertyMap
     * @brief Constructor
     */
    PropertyMap();
    /**
     * @fn ~PropertyMap
     * @brief Destructor
     */
    virtual ~PropertyMap() noexcept;
    /**
     * @fn getProperty
     * @brief Get the value under a section for the given key.
     * @param cr_section The section of the property
     * @param cr_key     The key to get the value for
     * @param cr_def_val The default value; defaults to empty string
     * @return the value for key in section if found, else the default value
     */
    const std::string getProperty(const std::string& cr_section,
            const std::string& cr_key, const std::string& cr_def_val = "") const;
    /**
     * @fn getSectionKv
     * @brief Get the key-value-map for a section.
     * @param cr_section The section to get the kv-map for
     * @return the kv-map if found, else an empty one
     */
    const keyValueMap& getSectionKv(const std::string& cr_section) const;
    /**
     * @fn addProperty
     * @brief Add a key-value pair for a section.
     * @note If the section does not exist, it gets created.
     * @param r_section The section name
     * @param r_kv      The key-value pair
     * @return true on success, else false
     */
    bool addProperty(const std::string& r_section, const keyValue& r_kv = { });

private:
    /// The map of sections with kv-maps
    std::unordered_map<std::string, keyValueMap> mPropertiesMap;
};

} /* namespace config */

#endif /* SRC_CONFIG_PROPERTYMAP_H_ */
