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

#include "PropertyMap.h"

#include <stdexcept>

namespace config
{
PropertyMap::PropertyMap()
{}

PropertyMap::~PropertyMap() noexcept
{}

const std::string PropertyMap::getProperty(const std::string& crSection,
                                           const std::string& crKey,
                                           const std::string& crDefault) const
{
    auto s_it = mPropertiesMap.find(crSection);
    if(s_it != mPropertiesMap.end())
    {
        auto it = s_it->second.find(crKey);
        if(it != s_it->second.end() && !it->second.empty())
        {
            return it->second;
        }
        else
        {
            return crDefault;
        }
    }
    else
    {
        return crDefault;
    }
}

const KeyValueMap& PropertyMap::getSectionKv(const std::string& crSection) const
{
    auto it = mPropertiesMap.find(crSection);
    if(it != mPropertiesMap.end())
    {
        return it->second;
    }
    else
    {
        throw std::out_of_range("section not found");
    }
}

bool config::PropertyMap::addProperty(const std::string& rSection,
                                      const KeyValue& rKeyValue)
{
    return mPropertiesMap[rSection].emplace(rKeyValue).second;
}

} /* namespace config */
