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

#include "ConfigReader.h"

#include <stdexcept>
#include <typeindex>
#include <utility>

#include "../util/Logger.h"

using namespace util;

namespace config
{

ConfigReader::ConfigReader()
        : mConfRE("^(\\S+?)\\s*?=\\s*?(\\S+?[^;]*?)\\s*?(?:;[\\S\\s]*?)?$",
                  boost::regex_constants::optimize)
{
}

ConfigReader::~ConfigReader() noexcept
{
}

void ConfigReader::read(std::istream& r_file)
{
    std::string key;
    std::string value;
    std::string line;
    std::string section;
    std::size_t line_nr = 0;
    while (std::getline(r_file, line))
    {
        line_nr++;
        try
        {
            if (line.length() == 0 || line.at(0) == ';')
            {
                continue;
            }
            if (line.at(0) == '[')
            {
                section = line.substr(1, line.rfind(']') - 1);
                mConfig.emplace(
                        std::make_pair(
                                section,
                                std::unordered_map<std::string, std::string>()));
                continue;
            }
            boost::smatch match;
            if (boost::regex_match(line, match, mConfRE))
            {
                key = match.str(1);
                value = match.str(2);
                std::size_t l = value.find_last_not_of(' ');
                if (l != std::string::npos)
                {
                    value = value.substr(0, l + 1);
                }
                mConfig[section].emplace(std::make_pair(key, value));
            } else
            {
                Logger::error(
                        "(ConfigReader) malformed param ["
                                + std::to_string(line_nr) + "]: ",
                        line);
            }
        } catch (const std::out_of_range& e)
        {
            continue;
        }
    }
}

const std::string ConfigReader::getProperty(const std::string& cr_section,
        const std::string& cr_key, const std::string& cr_def_val) const
{
    auto s_it = mConfig.find(cr_section);
    if (s_it != mConfig.end())
    {
        auto it = s_it->second.find(cr_key);
        if (it != s_it->second.end())
        {
            return it->second;
        } else
        {
            return cr_def_val;
        }
    } else
    {
        return cr_def_val;
    }
}

const std::unordered_map<std::string, std::string>& ConfigReader::getSectionKV(
        const std::string& cr_section) const
{
    auto it = mConfig.find(cr_section);
    if (it != mConfig.end())
    {
        return it->second;
    } else
    {
        throw std::out_of_range("section not found");
    }
}

}  // namespace config
