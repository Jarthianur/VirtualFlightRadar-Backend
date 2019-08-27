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

namespace config
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
            throw std::out_of_range(path + " is empty");
        }
        return p;
    }
    catch (ptree_bad_path const&)
    {
        throw std::out_of_range(path + " not found");
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
        throw std::out_of_range(section + " not found");
    }
}
}  // namespace config
