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

#include <unordered_map>

#include <boost/property_tree/ptree.hpp>

#include "error/IError.hpp"

#include "types.hpp"

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
    String Property(String const& path_, String const& def_) const noexcept;

    /**
     * Get a property at path (section.key).
     * @param path_ The property path
     * @return the value at path
     * @throw vfrb::config::error::CPropertyNotFoundError
     */
    String Property(String const& path_) const;

    /**
     * Get the properties for a section.
     * @param sect_ The section
     * @return the properties for that section
     * @throw vfrb::config::error::CPropertyNotFoundError
     */
    CProperties Section(String const& sect_) const;
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

    str Message() const noexcept override;
};
}  // namespace error
}  // namespace vfrb::config
