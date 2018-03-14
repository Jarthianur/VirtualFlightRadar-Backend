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

#include <cstddef>
#include <cstdint>
#include <list>
#include <sstream>
#include <string>
#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace config
{
namespace util
{
using Number    = boost::variant<std::int32_t, std::uint64_t, double>;
using OptNumber = boost::optional<Number>;

template<typename T>
inline OptNumber stringToNumber(const std::string& crStr)
{
    std::stringstream ss(crStr);
    T result;
    if(ss >> result)
    {
        return Number(result);
    }
    return boost::none;
}

/**
 * @fn trimString
 * @brief Trim a string on both sides.
 * @param rStr The string
 * @return the trimmed string
 */
inline std::string& trimString(std::string& rStr)
{
    std::size_t f = rStr.find_first_not_of(' ');
    if(f != std::string::npos)
    {
        rStr = rStr.substr(f);
    }
    std::size_t l = rStr.find_last_not_of(' ');
    if(l != std::string::npos)
    {
        rStr = rStr.substr(0, l + 1);
    }
    return rStr;
}

inline std::list<std::string> splitCommaSeparated(const std::string& crStr)
{
    std::list<std::string> list;
    std::stringstream ss;
    ss.str(crStr);
    std::string item;

    while(std::getline(ss, item, ','))
    {
        list.push_back(trimString(item));
    }
    return list;
}
}
}
