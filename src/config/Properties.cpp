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

#include "Properties.h"

#include <boost/property_tree/exceptions.hpp>

namespace config
{
Properties::Properties(const boost::property_tree::ptree& ptree) : m_pTree(ptree) {}

Properties::Properties(boost::property_tree::ptree&& ptree) : m_pTree(std::move(ptree)) {}

std::string Properties::get_property(const std::string& path, const std::string& alternative) const
{
    std::string prop(m_pTree.get(path, alternative));
    return prop.empty() ? alternative : prop;
}

Properties Properties::get_propertySection(const std::string& section) const
{
    try
    {
        return Properties(m_pTree.get_child(section));
    }
    catch (const boost::property_tree::ptree_bad_path&)
    {
        throw std::out_of_range(section + " not found");
    }
}

}  // namespace config
