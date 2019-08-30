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

#include "config/Properties.h"

#include <boost/property_tree/exceptions.hpp>

using namespace boost::property_tree;

namespace vfrb::config
{
Properties::Properties(ptree const& ptree) : m_pTree(ptree) {}

Properties::Properties(ptree&& ptree) : m_pTree(std::move(ptree)) {}

str Properties::property(str const& path, str const& defVal) const noexcept
{
    try
    {
        auto p = m_pTree.get_child(path).get_value<str>();
        return p.empty() ? defVal : p;
    }
    catch (ptree_bad_path const&)
    {
        return defVal;
    }
}

str Properties::property(str const& path) const
{
    try
    {
        auto p = m_pTree.get_child(path).get_value<str>();
        if (p.empty())
        {
            throw error::PropertyNotFoundError(path);
        }
        return p;
    }
    catch (ptree_bad_path const&)
    {
        throw error::PropertyNotFoundError(path);
    }
}

Properties Properties::section(str const& section) const
{
    try
    {
        return Properties(m_pTree.get_child(section));
    }
    catch (ptree_bad_path const&)
    {
        throw error::PropertyNotFoundError(section);
    }
}

namespace error
{
PropertyNotFoundError::PropertyNotFoundError(str const& prop) : m_property(prop + " not found") {}

char const* PropertyNotFoundError::what() const noexcept
{
    return m_property.c_str();
}
}  // namespace error
}  // namespace vfrb::config
