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
#include <boost/property_tree/ptree_fwd.hpp>

using boost::property_tree::ptree;
using boost::property_tree::ptree_bad_path;

namespace vfrb::config
{
CProperties::CProperties(ptree const& ptree_) : m_pTree(ptree_) {}

CProperties::CProperties(ptree&& ptree_) : m_pTree(ptree_) {}

auto CProperties::Property(String const& path_, String const& def_) const noexcept -> String {
    try {
        auto p = m_pTree.get_child(path_).get_value<String>();
        return p.empty() ? def_ : p;
    } catch ([[maybe_unused]] ptree_bad_path const&) {
        return def_;
    }
}

auto CProperties::Property(String const& path_) const -> String {
    try {
        auto p = m_pTree.get_child(path_).get_value<String>();
        if (p.empty()) {
            throw error::CPropertyNotFoundError(path_);
        }
        return p;
    } catch ([[maybe_unused]] ptree_bad_path const&) {
        throw error::CPropertyNotFoundError(path_);
    }
}

auto CProperties::Section(String const& sect_) const -> CProperties {
    try {
        return CProperties(m_pTree.get_child(sect_));
    } catch ([[maybe_unused]] ptree_bad_path const&) {
        throw error::CPropertyNotFoundError(sect_);
    }
}

namespace error
{
CPropertyNotFoundError::CPropertyNotFoundError(String const& prop_) : m_msg(prop_ + " not found") {}

auto CPropertyNotFoundError::Message() const noexcept -> str {
    return m_msg.c_str();
}
}  // namespace error
}  // namespace vfrb::config
