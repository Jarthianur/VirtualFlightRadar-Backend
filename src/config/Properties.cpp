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

#include <stdexcept>

namespace config
{
Properties::Properties() {}

Properties::~Properties() noexcept {}

const std::string Properties::get_property(const std::string& section, const std::string& key,
                                           const std::string& alternative) const
{
    auto sectionIt = m_propertiesMap.find(section);
    if (sectionIt != m_propertiesMap.end())
    {
        auto kvIt = sectionIt->second.find(key);
        if (kvIt != sectionIt->second.end() && !kvIt->second.empty())
        {
            return kvIt->second;
        }
        else
        {
            return alternative;
        }
    }
    else
    {
        return alternative;
    }
}

const KeyValueMap& Properties::get_propertySection(const std::string& section) const
{
    auto it = m_propertiesMap.find(section);
    if (it != m_propertiesMap.end())
    {
        return it->second;
    }
    else
    {
        throw std::out_of_range("section not found");
    }
}

bool config::Properties::addProperty(const std::string& section, const KeyValue& property)
{
    return m_propertiesMap[section].emplace(property).second;
}

}  // namespace config
