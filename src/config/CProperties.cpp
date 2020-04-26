/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "config/CProperties.hpp"

#include <boost/property_tree/exceptions.hpp>

using namespace boost::property_tree;

namespace vfrb::config
{
CProperties::CProperties(ptree const& ptree) : m_pTree(ptree) {}

CProperties::CProperties(ptree&& ptree) : m_pTree(std::move(ptree)) {}

String CProperties::Property(String const& path, String const& defVal) const noexcept {
    try {
        auto p = m_pTree.get_child(path).get_value<String>();
        return p.empty() ? defVal : p;
    } catch ([[maybe_unused]] ptree_bad_path const&) {
        return defVal;
    }
}

String CProperties::Property(String const& path) const {
    try {
        auto p = m_pTree.get_child(path).get_value<String>();
        if (p.empty()) {
            throw error::CPropertyNotFoundError(path);
        }
        return p;
    } catch ([[maybe_unused]] ptree_bad_path const&) {
        throw error::CPropertyNotFoundError(path);
    }
}

CProperties CProperties::Section(String const& section) const {
    try {
        return CProperties(m_pTree.get_child(section));
    } catch ([[maybe_unused]] ptree_bad_path const&) {
        throw error::CPropertyNotFoundError(section);
    }
}

namespace error
{
CPropertyNotFoundError::CPropertyNotFoundError(String const& prop_) : m_msg(prop_ + " not found") {}

str CPropertyNotFoundError::Message() const noexcept {
    return m_msg.c_str();
}
}  // namespace error
}  // namespace vfrb::config
