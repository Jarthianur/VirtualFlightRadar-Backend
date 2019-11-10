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
CProperties::CProperties(ptree const& ptree) : m_pTree(ptree) {}

CProperties::CProperties(ptree&& ptree) : m_pTree(std::move(ptree)) {}

Str CProperties::Property(Str const& path, Str const& defVal) const noexcept
{
    try
    {
        auto p = m_pTree.get_child(path).get_value<Str>();
        return p.empty() ? defVal : p;
    }
    catch (ptree_bad_path const&)
    {
        return defVal;
    }
}

Str CProperties::Property(Str const& path) const
{
    try
    {
        auto p = m_pTree.get_child(path).get_value<Str>();
        if (p.empty())
        {
            throw error::CPropertyNotFoundError(path);
        }
        return p;
    }
    catch (ptree_bad_path const&)
    {
        throw error::CPropertyNotFoundError(path);
    }
}

CProperties CProperties::Section(Str const& section) const
{
    try
    {
        return CProperties(m_pTree.get_child(section));
    }
    catch (ptree_bad_path const&)
    {
        throw error::CPropertyNotFoundError(section);
    }
}

namespace error
{
CPropertyNotFoundError::CPropertyNotFoundError(Str const& prop_) : m_msg(prop_ + " not found") {}

char const* CPropertyNotFoundError::Message() const noexcept
{
    return m_msg.c_str();
}
}  // namespace error
}  // namespace vfrb::config
