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

#include <string>
#include <unordered_map>

#include <boost/property_tree/ptree.hpp>

/// @namespace config
namespace config
{
/**
 * @class PropertyMap
 * @brief Store key-value pairs for sections.
 */
class Properties
{
public:
    explicit Properties(const boost::property_tree::ptree& ptree);

    explicit Properties(boost::property_tree::ptree&& ptree);

    ~Properties() noexcept = default;

    /**
     * @fn getProperty
     * @brief Get the value under a section for the given key.
     * @param crSection The section of the property
     * @param crKey     The key to get the value for
     * @param crDefault The default value (default: empty)
     * @return the value for key in section if found, else the default value
     */
    std::string get_property(const std::string& path, const std::string& alternative = "") const;

    /**
     * @fn getSectionKeyValue
     * @brief Get the key-value-map for a section.
     * @param crSection The section to get the kv-map for
     * @return the kv-map if found, else an empty one
     * @throw std::out_of_range if the section is not found
     */
    Properties get_propertySection(const std::string& section) const;

private:
    boost::property_tree::ptree m_pTree;
};

}  // namespace config
