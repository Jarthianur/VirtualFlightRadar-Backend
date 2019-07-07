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

namespace config
{
/**
 * @brief Store key-value pairs sectionwise.
 */
class Properties
{
    boost::property_tree::ptree m_pTree;  ///< The underlying property tree

public:
    explicit Properties(
        const boost::property_tree::ptree& ptree);             ///< @param ptree The property tree to copy
    explicit Properties(boost::property_tree::ptree&& ptree);  ///< @param ptree The property tree to move
    ~Properties() noexcept = default;

    /**
     * @brief Get the value at a property path (section.key), or a default value.
     * @param path        The property path
     * @param alternative The default value (default: empty)
     * @return the value at path if found and not empty, else the default value
     */
    std::string get_property(const std::string& path, const std::string& alternative = "") const;

    /**
     * @brief Get the Properties for a section.
     * @param section The section
     * @return the Properties for that section
     * @throw std::out_of_range if the section is not found
     */
    Properties get_propertySection(const std::string& section) const;
};

}  // namespace config
