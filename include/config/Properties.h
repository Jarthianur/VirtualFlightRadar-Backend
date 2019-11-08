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

#include "error/Error.hpp"

#include "types.h"

namespace vfrb::config
{
/**
 * @brief Store key-value pairs sectionwise.
 */
class CProperties
{
    boost::property_tree::ptree m_pTree;  ///< The underlying property tree

public:
    explicit CProperties(
        boost::property_tree::ptree const& ptree_);              ///< @param ptree The property tree to copy
    explicit CProperties(boost::property_tree::ptree&& ptree_);  ///< @param ptree The property tree to move
    ~CProperties() noexcept = default;

    /**
     * @brief Get the value at a property path (section.key), or a default value.
     * @param path        The property path
     * @param alternative The default value (default: empty)
     * @return the value at path if found and not empty, else the default value
     */
    Str Property(Str const& path_, Str const& defVal_) const noexcept;
    Str Property(Str const& path_) const;
    /**
     * @brief Get the Properties for a section.
     * @param section The section
     * @return the Properties for that section
     * @throw std::out_of_range if the section is not found
     */
    CProperties Section(Str const& section_) const;
};

namespace error
{
class CPropertyNotFoundError : public vfrb::error::IError
{
    Str const m_property;

public:
    explicit CPropertyNotFoundError(Str const& prop_);
    ~CPropertyNotFoundError() noexcept override = default;

    char const* What() const noexcept override;
};
}  // namespace error
}  // namespace vfrb::config
