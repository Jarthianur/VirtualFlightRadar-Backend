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

#pragma once

#include <unordered_map>

#include <boost/property_tree/ptree.hpp>

#include "error/IError.hpp"

#include "Types.hpp"

namespace vfrb::config
{
/// An INI format property store
class CProperties
{
    boost::property_tree::ptree m_pTree;  ///< The underlying property tree

public:
    explicit CProperties(boost::property_tree::ptree const& ptree_);
    explicit CProperties(boost::property_tree::ptree&& ptree_);

    /**
     * Get a property at path (section.key).
     * @param path_ The property path
     * @param def_  The default value
     * @return the value at path if found, else the default value
     */
    [[nodiscard]] auto Property(String const& path_, String const& def_) const noexcept -> String;

    /**
     * Get a property at path (section.key).
     * @param path_ The property path
     * @return the value at path
     * @throw vfrb::config::error::CPropertyNotFoundError
     */
    [[nodiscard]] auto Property(String const& path_) const -> String;

    /**
     * Get the properties for a section.
     * @param sect_ The section
     * @return the properties for that section
     * @throw vfrb::config::error::CPropertyNotFoundError
     */
    [[nodiscard]] auto Section(String const& sect_) const -> CProperties;
};

namespace error
{
/// Error to indicate that a property was not found or is empty
class CPropertyNotFoundError : public vfrb::error::IError
{
    String const m_msg;

public:
    /// @param prop_ The property name
    explicit CPropertyNotFoundError(String const& prop_);

    [[nodiscard]] auto Message() const noexcept -> str override;
};
}  // namespace error
}  // namespace vfrb::config
